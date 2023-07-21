#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QDate>
#include <QIODevice>
#include "qcustomplot.h"
#include "mythread.h"
#include <QObject>
#include <QBrush>
#include <QLinearGradient>
#include <QInputDialog>

// 采样频率
extern uint16_t freq;
// 采样点数目
extern uint16_t freq_num;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置UI标题
    this->setWindowTitle("Rotor vibration measurement system");
    // 扫描本机可用串口，添加到"串口选择"下拉框中
    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
        ui->portSelect->addItem(info.portName());
    }

    // qDebug() << "MainWindow线程id: " << QThread::currentThread(); // 测试

    /* 创建线程对象 */
    // 1个线程 用于数据接收和处理
    this->m_t1 = new QThread;
    // 1个线程 用于数据FFT分析
    this->m_t2 = new QThread;
    // 1个线程 用于数据存储
    this->m_t3 = new QThread;

    // 创建数据接收和处理任务类的对象
    this->m_data_recv = new DataReceive;
    // 创建数据分析任务类对象
    this->m_data_analyse = new DataAnalyse;
    // 创建数据存储任务类对象
    this->m_data_store = new DataStore;

    // 将数据接收和处理任务类对象移动到子线程中
    this->m_data_recv->moveToThread(this->m_t1);
    // 将数据分析任务类对象移动到子线程中
    this->m_data_analyse->moveToThread(this->m_t2);
    // 将数据存储任务类对象移动到子线程中
    this->m_data_store->moveToThread(this->m_t3);
    // 开启子线程
    this->m_t1->start();
    this->m_t2->start();
    this->m_t3->start();

    // 初始化串口实例
    this->m_data_recv->m_port = new QSerialPort(this);

    /******************************************/
    // 主线程通知数据分析线程执行while循环，进行数据分析
    connect(this, &MainWindow::notify_start_analyse, this->m_data_analyse, &DataAnalyse::slot_anlyse_data);
    // 主线程通知子线程数据库连接
    connect(this, &MainWindow::notify_db_connect, this->m_data_store, &DataStore::slot_conn_db);
    /******************************************/

    /******************************************/
    // 数据分析线程通知主线程进行波形绘制
    connect(this->m_data_analyse, &DataAnalyse::notify_play_data, this, &MainWindow::slot_line_plot);
    // 数据分析线程通知数据库存储线程进行数据存储
    connect(this->m_data_analyse, &DataAnalyse::notify_store_data, this->m_data_store, &DataStore::slot_store_data);
    /******************************************/

    // 向状态栏添加标签用于显示FPS和总数据量
    QLabel* label = new QLabel(this);
    ui->statusBar->addWidget(label);

    // 初始化customPlot绘图工具
    customPlot_config();


    /*********** 初始化采集按钮标志 **************/
    this->m_on_collect = false;  // 该按钮不能失能，否则无法开始或停止采集
    // 开始采集按钮失能
    ui->start_collect->setDisabled(true);
    // 清空DB按钮失能
    ui->clear_db->setDisabled(true);
    // 导出数据按钮失能
    ui->export_data->setDisabled(true);
    // 汉宁窗、海明窗按钮使能
    ui->hanning->setEnabled(true);
    ui->hamming->setEnabled(true);
    /******************************************/

    // 发射数据存储线程启动信号  直接让数据处理和分析线程开始进入while循环
    emit this->notify_start_analyse();

    // 析构子线程对象  当MainWindow结束之后就析构这些线程对象
    connect(this, &MainWindow::destroyed, this, [=](){
        this->m_t1->quit();
        this->m_t1->wait();
        this->m_t1->deleteLater();  // delete t1也可以
        this->m_data_recv->deleteLater();
        this->m_t2->quit();
        this->m_t2->wait();
        this->m_t2->deleteLater();  // delete t2也可以
        this->m_data_analyse->deleteLater();
        this->m_t3->quit();
        this->m_t3->wait();
        this->m_t3->deleteLater();  // delete t3也可以
        this->m_data_store->deleteLater();
    });

    // 默认不加窗函数
    this->m_win_index = 0;
}

/*
*	@brief:  配置波形图参数
*	@param:  none
*	@return: void
*/
void MainWindow::customPlot_config(){

    // 图表添加两条曲线
    ui->customPlot_one->addGraph();
    ui->customPlot_two->addGraph();
    ui->fft_one->addGraph();
    ui->fft_two->addGraph();
    ui->axisOrbit->addGraph();
    // 设置曲线颜色
    ui->customPlot_one->graph(0)->setPen(QPen(Qt::blue));
    ui->customPlot_two->graph(0)->setPen(QPen(Qt::blue));
    ui->fft_one->graph(0)->setPen(QPen(Qt::blue));
    ui->fft_two->graph(0)->setPen(QPen(Qt::blue));
    ui->axisOrbit->graph(0)->setPen(QPen(Qt::blue));
    // 设置坐标轴名称
    ui->customPlot_one->xAxis->setLabel("dataCount");
    ui->customPlot_two->xAxis->setLabel("dataCount");
    ui->fft_one->xAxis->setLabel("x-frequency");        // x-频率
    ui->fft_two->xAxis->setLabel("y-frequency");        // y-频率
    ui->axisOrbit->xAxis->setLabel("axis orbit");       // 轴心轨迹
    // 设置其他方向的轴的可见性
    ui->customPlot_one->xAxis2->setVisible(true);
    ui->customPlot_one->yAxis2->setVisible(true);
    ui->customPlot_two->xAxis2->setVisible(true);
    ui->customPlot_two->yAxis2->setVisible(true);
    ui->fft_one->xAxis2->setVisible(true);
    ui->fft_one->yAxis2->setVisible(true);
    ui->fft_two->xAxis2->setVisible(true);
    ui->fft_two->yAxis2->setVisible(true);
    ui->axisOrbit->xAxis2->setVisible(true);
    ui->axisOrbit->yAxis2->setVisible(true);
    // 设置是否显示轴刻度
    ui->customPlot_one->xAxis2->setTickLabels(false);
    ui->customPlot_one->yAxis2->setTickLabels(false);
    ui->customPlot_two->xAxis2->setTickLabels(false);
    ui->customPlot_two->yAxis2->setTickLabels(false);
    ui->fft_one->xAxis2->setTickLabels(false);
    ui->fft_one->yAxis2->setTickLabels(false);
    ui->fft_two->xAxis2->setTickLabels(false);
    ui->fft_two->yAxis2->setTickLabels(false);
    ui->axisOrbit->xAxis2->setTickLabels(false);
    ui->axisOrbit->yAxis2->setTickLabels(false);

    // 允许用户用鼠标拖动轴范围，用鼠标滚轮缩放，点击选择图形
    ui->customPlot_one->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot_two->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->fft_one->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->fft_two->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->axisOrbit->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

/*
*	@brief:  打开/关闭串口 按钮的plot
*	@param:  none
*	@return: void
*/
void MainWindow::on_openPort_clicked(){
    static int status = 1;
    // qDebug() << "status: " << status;  测试
    if(status){
        // 设置串口名称
        this->m_data_recv->m_port->setPortName(ui->portSelect->currentText());
        // 设置波特率
        this->m_data_recv->m_port->setBaudRate(ui->baudRate->currentText().toInt());
        // 设置停止位
        this->m_data_recv->m_port->setStopBits(QSerialPort::StopBits(ui->stopBits->currentText().toInt()));
        // 设置数据位
        this->m_data_recv->m_port->setDataBits(QSerialPort::DataBits(ui->dataBits->currentText().toInt()));
        // 设置校验位
        switch(ui->parity->currentIndex()){
            case 0:
                this->m_data_recv->m_port->setParity(QSerialPort::NoParity);
            break;
            case 1:
                this->m_data_recv->m_port->setParity(QSerialPort::EvenParity);
            break;
            case 2:
                this->m_data_recv->m_port->setParity(QSerialPort::OddParity);
            break;
            case 3:
                this->m_data_recv->m_port->setParity(QSerialPort::SpaceParity);
            break;
            case 4:
                this->m_data_recv->m_port->setParity(QSerialPort::MarkParity);
            break;
            default:
                break;
        }
        // 设置流控 无流控
        this->m_data_recv->m_port->setFlowControl(QSerialPort::NoFlowControl);
        // 读写方式打开串口
        if(!this->m_data_recv->m_port->open(QIODevice::ReadWrite)){
            QMessageBox::warning(this, "defeat", "Open port defeat");
        }

        else{

            QMessageBox::information(this, "success", "Open port success!");
            // 使能开始采集按钮
            ui->start_collect->setEnabled(true);
            // 将打开串口设置为关闭串口
            ui->openPort->setText("ClosePort");
            // 将各个选项设置为不可选状态  打开串口之后其它串口操作不能进行
            ui->portSelect->setEnabled(false);
            ui->baudRate->setEnabled(false);
            ui->stopBits->setEnabled(false);
            ui->dataBits->setEnabled(false);
            ui->parity->setEnabled(false);
            status = 0;
            // 连接接收数据信号  连接信号之后 有数据到来 子线程执行readData  子线程进行串口数据的接收与处理
            connect(this->m_data_recv->m_port, &QSerialPort::readyRead, this->m_data_recv, &DataReceive::slot_readData);
        }
    }
    else{
        // 关闭串口
        this->m_data_recv->m_port->close();
        // 将各个串口选项设置为可选状态
        ui->portSelect->setEnabled(true);
        ui->baudRate->setEnabled(true);
        ui->stopBits->setEnabled(true);
        ui->dataBits->setEnabled(true);
        ui->parity->setEnabled(true);
        // 将关闭串口设置为打开串口
        ui->openPort->setText("OpenPort");

        QMessageBox::information(this, "success", "Close port success!");
        // 失能开始采集按钮
        ui->start_collect->setDisabled(true);
        status = 1;
        // 断开接收数据信号连接
        disconnect(this->m_data_recv->m_port, &QSerialPort::readyRead, this->m_data_recv, &DataReceive::slot_readData);
    }
}

/*
*	@brief:  开始/停止采集 按钮slot
*	@param:  none
*	@return: void
*/
void MainWindow::on_start_collect_clicked(){
    // 第一次进入 on_collect为false
    if(this->m_on_collect == false){

        // 处理label中的采样频率和采样点数数据进行发送给下位机
        QByteArray HEX_DATA = data_convert(0xAA);

        // 根据 label中的采样频率和采样点数数据 设置x、y轴坐标显示范围
        ui->customPlot_one->xAxis->setRange(0, freq_num);
        ui->customPlot_one->yAxis->setRange(-5.0, 5.0);

        ui->customPlot_two->xAxis->setRange(0, freq_num);
        ui->customPlot_two->yAxis->setRange(-5.0, 5.0);

        ui->fft_one->xAxis->setRange(0, freq / 4);
        ui->fft_one->yAxis->setRange(0, 5);

        ui->fft_two->xAxis->setRange(0, freq / 4);
        ui->fft_two->yAxis->setRange(0, 5);

        ui->axisOrbit->xAxis->setRange(-5.0, 5.0);
        ui->axisOrbit->yAxis->setRange(-10.0, 10.0);

        ui->start_collect->setText("Stop");

        // 给下位机发送指令 FA AA High_freq Low_freq High_freq_num Low_freq_num FB
        this->m_data_recv->m_port->write(HEX_DATA);
        // qDebug() << "Send data:" << HEX_DATA; // 测试
        // qDebug() << "Start collect"; // 测试
        this->m_on_collect = true;
        // 清空DB按钮 导出数据按钮 连接数据库按钮 汉宁窗按钮 海明窗按钮 失能  开始采集时，不能够进行其他操作
        ui->clear_db->setDisabled(true);
        ui->export_data->setDisabled(true);
        ui->connect_db->setDisabled(true);
        ui->hanning->setDisabled(true);
        ui->hamming->setDisabled(true);
    }
    else{

        // 停止采集指令发送给下位机
        QByteArray HEX_DATA = data_convert(0xBB);
        // 给下位机发送指令 FA BB High_freq Low_freq High_freq_num Low_freq_num FB
        this->m_data_recv->m_port->write(HEX_DATA);
        // qDebug() << "Send data:" << HEX_DATA; // 测试
        // qDebug() << "Stop collect";           // 测试

        this->m_on_collect = false;
        ui->start_collect->setText("Start");
        // 如果数据库没有连接
        if(!this->m_sql.if_on_connect()){
            // 之前没有经过加窗 全部都使能
            if(this->m_win_index == 0){
                ui->hanning->setEnabled(true);
                ui->hamming->setEnabled(true);
                ui->connect_db->setEnabled(true);
            }
            // 之前加过汉宁窗，hanning按钮使能
            else if(this->m_win_index == 1){
                ui->hanning->setEnabled(true);
            }
            // 之前加过海明窗，hamming按钮使能
            else if(this->m_win_index == 2){
                ui->hamming->setEnabled(true);
            }
        }
        // 数据库连接着 可以进行数据库清空、导出数据
        else{
            // 清空DB按钮 导出数据按钮 连接数据库按钮 使能 可以进行点击
            ui->connect_db->setEnabled(true);
            ui->clear_db->setEnabled(true);
            ui->export_data->setEnabled(true);
        }
    }
}

/*
*	@brief:  绘图
*	@param:  图数据
*	@return: void
*/
void MainWindow::slot_line_plot(QVector<double> channel_one, QVector<double> channel_two,
                           QVector<double> data_power_one, QVector<double> data_power_two,
                           QVector<double> data_freq_one, QVector<double> data_freq_two)
{
    static long long count = 0;
    ui->customPlot_one->graph(0)->setData(this->m_x, channel_one);
    ui->customPlot_two->graph(0)->setData(this->m_x, channel_two);
    ui->fft_one->graph(0)->setData(data_freq_one, data_power_one);
    ui->fft_two->graph(0)->setData(data_freq_two, data_power_two);
    ui->axisOrbit->graph(0)->setData(channel_one, channel_two);

    ui->customPlot_one->replot();
    ui->customPlot_two->replot();
    ui->fft_one->replot();
    ui->fft_two->replot();
    ui->axisOrbit->replot();
    // 显示已读取的数据总量
    count += channel_one.size() + channel_two.size();
    ui->statusBar->showMessage(QString("total data: %1").arg(count));
}

/*
*	@brief:  计算汉宁窗
*	@param:  窗的数据总量与长度
*	@return: void
*/
void MainWindow::hanning(QVector<double> &win, int len)
{
    double a = 0.5;
    double pi = 3.1415926;
    for(int i = 0; i < len; ++i){
        // 加窗之后会导致频谱图的幅值下降 -> 幅值相等恢复系数为2，
        win[i] = 2 * (a - (1.0f - a) * cos(2 * pi * i / (len - 1)));
    }
}

/*
*	@brief:  计算海明窗
*	@param:  窗的数据总量与长度
*	@return: void
*/
void MainWindow::hamming(QVector<double> &win, int len)
{
    double a = 0.53836;
    double pi = 3.1415926;
    for(int i = 0; i < len; ++i){
        // 加窗之后会导致频谱图的幅值下降 -> 幅值相等恢复系数为1.852，
        win[i] = 1.852 * (a - (1.0f - a) * cos(2 * pi * i / (len - 1)));
    }
}

/*
*	@brief:  label数字转换，构建指令数据包
*	@param:  开始采集/停止采集指令
*	@return: void
*/
QByteArray MainWindow::data_convert(uint8_t order)
{
    // 从UI获得freq
    freq = ui->freq->text().toUShort();
    // 从UI获得freq_num
    freq_num = ui->freq_num->text().toUShort();

    if(this->m_win_index == 1){
        // 计算汉宁窗
        this->m_data_analyse->m_win.resize(freq_num);
        this->hanning(this->m_data_analyse->m_win, freq_num);
        // 将加窗标志位置1
        this->m_data_analyse->m_win_flag = true;
    }
    else if(this->m_win_index == 2){
        // 计算海明窗
        this->m_data_analyse->m_win.resize(freq_num);
        this->hamming(this->m_data_analyse->m_win, freq_num);
        // 将加窗标志位置1
        this->m_data_analyse->m_win_flag = true;
    }
    else{
        // 将加窗标志位置0
        this->m_data_analyse->m_win_flag = false;
    }
    // 更新时域x坐标
    this->m_x.clear();
    for(int i = 0; i < freq_num; ++i){
        this->m_x.push_back(i);
    }
    uint8_t High_freq, Low_freq, High_freq_num, Low_freq_num;
    // 高8位
    High_freq = (freq >> 8) & 0xFF;
    // 低8位
    Low_freq = freq & 0xFF;
    // 高8位
    High_freq_num = (freq_num >> 8) & 0xFF;
    // 低8位
    Low_freq_num = freq_num & 0xFF;
    QByteArray HEX_data;
    HEX_data.push_back(0xFA);
    HEX_data.push_back(order);
    HEX_data.push_back(High_freq);
    HEX_data.push_back(Low_freq);
    HEX_data.push_back(High_freq_num);
    HEX_data.push_back(Low_freq_num);
    HEX_data.push_back(0xFB);
    return HEX_data;
}

/*
*	@brief:  使用汉宁窗进行频谱分析 触发信号
*	@param:  none
*	@return: void
*/
void MainWindow::on_hanning_clicked()
{
    static bool flag = false;
    if(flag == false){
        this->m_win_index = 1;
        ui->hanning->setText("Cancel");
        ui->hamming->setDisabled(true);
        flag = true;
        ui->connect_db->setDisabled(true);
    }
    else{
        this->m_win_index = 0;
        ui->hanning->setText("Hanning");
        ui->hamming->setEnabled(true);
        flag = false;
        ui->connect_db->setEnabled(true);
    }
}

/*
*	@brief:  使用海明窗进行频谱分析 触发信号
*	@param:  none
*	@return: void
*/
void MainWindow::on_hamming_clicked()
{
    static bool flag = false;
    if(flag == false){
        this->m_win_index = 2;
        ui->hamming->setText("Cancel");
        ui->hanning->setDisabled(true);
        flag = true;
        // 连接数据库标志位disabled
        ui->connect_db->setDisabled(true);
    }
    else{
        this->m_win_index = 0;
        ui->hamming->setText("Hamming");
        ui->hanning->setEnabled(true);
        flag = false;
        // 连接数据库标志位enabled
        ui->connect_db->setEnabled(true);
    }
}

/*
*	@brief:  连接/关闭数据库  触发信号
*	@param:  none
*	@return: void
*/
void MainWindow::on_connect_db_clicked(){

    // qDebug() << "on_connect_db_clicked thread_id: " << QThread::currentThread(); // 测试

    // 如果没有点击"开始采集"（其实这里不用判断）因为点击"开始采集"之后，"连接数据库"按钮就失能了
    if(!this->m_on_collect){
        // 通知子线程去连接数据
        if(!this->m_sql.if_on_connect()){
            bool flag = this->m_sql.connect_db("operator db");

            if(flag == false){
                QMessageBox::warning(this, "error", "Connect DB defeat");
            }
            else if(flag){
                // 通知子线程进行数据库连接
                emit this->notify_db_connect(true);
                QMessageBox::information(this, "success", "measure DB connect success!");
                ui->connect_db->setText("CloseDB");
                // 清空DB按钮 导出数据按钮 使能
                ui->clear_db->setEnabled(true);
                ui->export_data->setEnabled(true);
                // 连接数据库时不能加窗函数分析
                ui->hanning->setDisabled(true);
                ui->hamming->setDisabled(true);
            }
        }
        else{
            // 通知子线程关闭数据库
            emit this->notify_db_connect(false);
            // 关闭数据库
            this->m_sql.close_db();
            QMessageBox::information(this, "success", "measure DB close success!");
            ui->connect_db->setText("connected");
            // 清空DB按钮 导出数据按钮 失能
            ui->clear_db->setDisabled(true);
            ui->export_data->setDisabled(true);

            ui->hanning->setEnabled(true);
            ui->hamming->setEnabled(true);
        }
    }
}

/*
*	@brief:  清空数据库 触发信号
*	@param:  none
*	@return: void
*/
void MainWindow::on_clear_db_clicked()
{
    if(this->m_sql.if_on_connect()){
        QString query = "truncate table measure_data";
        bool flag = false;
        flag = this->m_sql.sql_query(query);
        if(flag){
            qDebug() << "Clear DB success!";
            QMessageBox::information(this, "success", "Clear DB success!");
        }
        else{
            qDebug() << "Clear DB defeat: " << this->m_sql.get_error();
            QMessageBox::warning(this, this->m_sql.get_error(), "Clear DB defeat");
        }
    }
}

/*
*	@brief:  导出数据 触发信号
*	@param:  none
*	@return: void
*/
void MainWindow::on_export_data_clicked()
{
    // qDebug() << "export_db_data thread_id: " << QThread::currentThread();  //  测试
    if(this->m_sql.if_on_connect()){
        QString query = QString("select * from measure_data into outfile 'E:/task/measure_system/mysql_data/%1_%2_data.csv' "
                                "fields terminated by ',' lines terminated by '\n'")
                                .arg(QDate::currentDate().toString("yyyy-MM-dd"),
                                     QTime::currentTime().toString("hh-mm-ss")
                                );
        qDebug() << query;

        bool flag = false;
        flag = this->m_sql.sql_query(query);
        if(flag){
            qDebug() << "export DB success!";
            QMessageBox::information(this, "success", "export DB success!");
        }
        else{
            qDebug() << "export DB defeat" << this->m_sql.get_error();
            QMessageBox::warning(this, this->m_sql.get_error(), "export DB defeat");
        }
    }
}

MainWindow::~MainWindow(){
    delete ui;
}




