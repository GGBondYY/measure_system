#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>  // 串口类
#include <QSerialPortInfo>  // 串口信息类
#include <QQueue>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include "mythread.h"
#include "qmysql.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_openPort_clicked();                   // 打开串口

    void on_start_collect_clicked();              // 开始采集

    void on_connect_db_clicked();                 // 连接数据库

    void on_clear_db_clicked();                   // 清空数据库

    void on_export_data_clicked();                // 数据库导出数据

                                                  // 绘制数据
    void slot_line_plot(QVector<double> channel_one, QVector<double> channel_two,
                   QVector<double> data_power_one, QVector<double> data_power_two,
                   QVector<double> data_freq_one, QVector<double> data_freq_two);
    QByteArray data_convert(uint8_t order);


    void on_hanning_clicked();

    void on_hamming_clicked();

private:

    void customPlot_config();                       //  配置customPlot
    void hanning(QVector<double>& win, int len);    // 计算汉宁窗
    void hamming(QVector<double>& win, int len);    // 计算海明窗
signals:
    void notify_start_analyse();                    // 开始存储
    void notify_db_connect(bool ntf);               // 子线程数据库连接和断开信号
private:
    Ui::MainWindow *ui;                             // 窗口实例

    bool m_on_collect;                              // 记录采集开始和停止

    DataReceive* m_data_recv;                       // 数据接收实例
    DataAnalyse* m_data_analyse;                    // 数据处理和分析实例
    DataStore* m_data_store;                        // 数据存储实例

    QThread* m_t1;                                  // 用于数据接收和处理
    QThread* m_t2;                                  // 用于数据分析
    QThread* m_t3;                                  // 用于数据存储

    qmysql m_sql;                                   // 数据库连接实例
    QVector<double> m_x;                            // x轴坐标点
    int m_win_index;                                // 选择窗函数 0:不加窗 1:汉宁窗 2:海明窗

};
#endif // MAINWINDOW_H
