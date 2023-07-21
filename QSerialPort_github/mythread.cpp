#include "mythread.h"
#include <QDebug>

// 生产者消费者缓冲区 不考虑缓冲区满
QQueue<double> FIFO;
// 互斥锁
QMutex m_mutex;
// 条件变量
QWaitCondition m_cond;
// 采样频率
uint16_t freq;
// 采样点数目
uint16_t freq_num;
// 数据分析和数据存储线程之间数据库连接成功记号
bool fl = false;

/******************************************************/
/*******************数据接收和处理任务类********************/
/******************************************************/
DataReceive::DataReceive(QObject *parent)
    : QObject{parent}
{

}

/*
*	@brief:  接收串口数据  子线程
*	@param:  void
*	@return: void
*/
void DataReceive::slot_readData(){

    // qDebug() << "slot_readData thread_id: " << QThread::currentThread();  // 测试

    // 从串口读取数据追加到INFO中
    m_info.append(this->m_port->readAll());

    int info_size = m_info.size();
    // 每次处理4字节数据的整数倍   接收到的一个数据包为4字节
    int count = info_size / 4;
    for(int i = 0; i < count; ++i){

        QByteArray f_info = m_info.mid(0, 4);  // 从info的位置0截取4个字节
        m_info.remove(0, 4);                   // 删除4个字节数据

        // 解析收到的串口数据  包头和包尾
        if(f_info.size() == 4){
            if(static_cast<uint8_t>(f_info[0]) == 0XFA && static_cast<uint8_t>(f_info[3]) == 0XFB){
                uint16_t data = static_cast<uint8_t>(f_info[1]) * 256 + static_cast<uint8_t>(f_info[2]);
                int val;
                // 判断±
                val = data & 0x8000 ? (-1 * ((~data + 1) & 0x7FFF)) : data;
                // 转为AD7606输入大小
                double adc_val = 5.0 * val / 32768.0;
                qDebug() << "adc_val:" << adc_val;  // 测试
                // 放置FIFO缓冲区  临界资源
                m_mutex.lock();
                FIFO.push_back(adc_val);
                m_cond.notify_one();
                m_mutex.unlock();
            }
        }
    }
}

/*************************************************/
/*******************数据分析任务类********************/
/*************************************************/

DataAnalyse::DataAnalyse(QObject *parent)
    : QObject{parent}
{
    this->m_db_stroe_index = 0;
    this->m_two_channel_done = 0;
    this->m_freq_nums_done = 0;
    this->m_channel_one.clear();
    this->m_channel_two.clear();
    this->m_win_flag = false;
}

/*
*	@brief:  数据处理和分析
*	@param:  void
*	@return: void
*/
void DataAnalyse::slot_anlyse_data(){

    // qDebug() << "anlyse_and_store_data thread_id: " << QThread::currentThread(); // 测试
    // 记录已分析数据数量
    static int num = 0;
    // 循环等待从FIFO中取出数据
    while(1){
        m_mutex.lock();
        while(FIFO.empty()){             
            m_cond.wait(&m_mutex);  // 当没有数据时，wait释放锁并阻塞等待 有数据到来时，自动加锁向下执行
        }
        double data = FIFO.front(); // 从头读取出一个数据
        // qDebug() << "data:" << data;  测试
        FIFO.pop_front();
        m_mutex.unlock();
        // 一通道
        if(this->m_db_stroe_index == 0){
            this->m_channel_one.push_back(data);  // 向通道一添加数据
            this->m_db_stroe_index = 1;
        }
        // 二通道
        else if(this->m_db_stroe_index == 1){
            this->m_channel_two.push_back(data);  // 向通道二添加数据
            this->m_db_stroe_index = 0;
            this->m_two_channel_done = 1;
            num++;
            if(num == freq_num){
                this->m_freq_nums_done = 1;     // 一次采样点数 数据读取完
                num = 0;
            }
        }
        // 两通道数据读取完
        if(this->m_two_channel_done == 1){
            // 如果数据库连接成功，通知数据库存储线程存储两通道数据
            if(fl){
                emit this->notify_store_data(this->m_channel_one.back(), this->m_channel_two.back());
            }

            // 如果采样点数达到了，就对其进行FFT分析
            if(this->m_freq_nums_done == 1){
                // fft分析
                this->fft_analyse(this->m_channel_one, this->m_channel_two, freq, freq_num);
                this->m_freq_nums_done = 0;
                this->m_channel_one.clear();
                this->m_channel_two.clear();
            }
            this->m_two_channel_done = 0;
        }
    }
}

/*
*	@brief:  fft分析
*	@param:  channel_one:通道一所有数据 channel_two:通道二所有数据
*	@return: void
*/
void DataAnalyse::fft_analyse(QVector<double> channel_one, QVector<double> channel_two, int freq, int freq_num)
{
    // qDebug() << "fft_analyse thread_id: " << QThread::currentThread(); // 测试
    int FFTWN = freq_num;  // 采集点数目
    int FFTWNFs = freq;    // 采集频率

    double *in_one;
    double *in_two;
    fftw_complex *out_one;
    fftw_complex *out_two;
    fftw_plan my_plan_one;
    fftw_plan my_plan_two;
    in_one = (double *)fftw_malloc(sizeof( double) * FFTWN);
    out_one = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * FFTWN);
    in_two = (double *)fftw_malloc(sizeof( double) * FFTWN);
    out_two = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * FFTWN);
    // 经过加窗处理
    if(this->m_win_flag){
        for (int i = 0; i < FFTWN; ++i)
        {
            in_one[i] = channel_one[i] * this->m_win[i];
            in_two[i] = channel_two[i] * this->m_win[i];
        }
    }
    // 不经过加窗处理
    else{
        for (int i = 0; i < FFTWN; ++i)
        {
            in_one[i] = channel_one[i];
            in_two[i] = channel_two[i];
        }
    }
    my_plan_one = FFTW3_H::fftw_plan_dft_r2c_1d(FFTWN, in_one, out_one, FFTW_ESTIMATE);
    fftw_execute(my_plan_one);

    my_plan_two = FFTW3_H::fftw_plan_dft_r2c_1d(FFTWN, in_two, out_two, FFTW_ESTIMATE);
    fftw_execute(my_plan_two);

    QVector<double> data_power_one;
    QVector<double> data_freq_one;

    QVector<double> data_power_two;
    QVector<double> data_freq_two;

    for( int i = 0; i < FFTWN/2+1; i++)
    {
        data_power_one.append((out_one[i][0] * out_one[i][0] + out_one[i][1] * out_one[i][1]) / (FFTWNFs*FFTWN));
        data_freq_one.append((double)FFTWNFs / FFTWN*i);
        data_power_two.append((out_two[i][0] * out_two[i][0] + out_two[i][1] * out_two[i][1]) / (FFTWNFs*FFTWN));
        data_freq_two.append((double)FFTWNFs / FFTWN*i);
    }

    fftw_destroy_plan(my_plan_one);
    fftw_destroy_plan(my_plan_two);

    fftw_free(in_one);
    fftw_free(in_two);

    fftw_free(out_one);
    fftw_free(out_two);

    // 通知主线程进行数据显示
    emit notify_play_data(channel_one, channel_two, data_power_one, data_power_two, data_freq_one, data_freq_two);
}

/*************************************************/
/*******************数据存储任务类********************/
/*************************************************/

DataStore::DataStore(QObject *parent)
            : QObject{parent}
{

}

/*
*	@brief:  存储数据
*	@param:  cn_one:通道一数据  cn_two:通道二数据
*	@return: void
*/
void DataStore::slot_store_data(double cn_one, double cn_two)
{
    // qDebug() << "slot_store_data thread_id: " << QThread::currentThread(); // 测试
    // 进行数据库操作
    if(this->m_sql.if_on_connect()){
        QString query = QString("insert into measure_data (passage_one, passage_two, date, time) values(%1, %2, '%3', '%4')")
                      .arg(cn_one)
                      .arg(cn_two)
                      .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                      .arg(QTime::currentTime().toString("hh:mm:ss"));

        this->m_sql.sql_query(query);
    }
}

/*
*	@brief:  连接/关闭数据库
*	@param:  ntf:连接/关闭数据库用标志位
*	@return: void
*/
void DataStore::slot_conn_db(bool ntf)
{
    // qDebug() << "slot conn_db thread_id: " << QThread::currentThread();  // 测试
    if(ntf){
        bool flag = false;
        flag = this->m_sql.connect_db("store data");
        if(flag == true){
            qDebug() << "store data数据库连接成功";
            fl = true;  // 用于给数据处理和分析线程判断是否需要存储数据
        }
        else{
            qDebug() << "store data数据库连接失败";
            fl = false;
        }
    }
    else{
        qDebug() << "store data数据库关闭成功";
        this->m_sql.close_db();
        fl = false;
    }
}
