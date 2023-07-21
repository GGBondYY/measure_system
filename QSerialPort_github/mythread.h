#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QQueue>
#include <QTime>
#include <QSerialPort>
#include <QString>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "qmysql.h"
#include <QVector>
#include "fftw3.h"

// 数据接收和处理任务类
class DataReceive : public QObject
{
    Q_OBJECT
public:
    explicit DataReceive(QObject *parent = nullptr);

public slots:
    void slot_readData();                // 接收串口数据
signals:    

public:
    QSerialPort* m_port;                 // 串口实例
    QByteArray m_info;                   // 接收串口的info缓冲区
};

// 数据分析任务类
class DataAnalyse : public QObject
{
    Q_OBJECT
public:
    explicit DataAnalyse(QObject *parent = nullptr);

public slots:
    void slot_anlyse_data();                                // 处理数据
public:
    void fft_analyse(QVector<double> channel_one,
                     QVector<double> channel_two,
                     int freq, int freq_num);               // fft分析
signals:
    void notify_play_data(QVector<double> channel_one,
                          QVector<double> channel_two,
                          QVector<double> data_power_one,
                          QVector<double> data_power_two,
                          QVector<double> data_freq_one,
                          QVector<double> data_freq_two);   // 通知主线程绘制数据
    void notify_store_data(double cn_one, double cn_two);   // 通知存储线程存储数据
public:
    int m_db_stroe_index;               // 进行数据库存储用的标识
    int m_two_channel_done;             // 读取了两通道的标志位
    int m_freq_nums_done;               // 读取了freq_num个数据
    QVector<double> m_channel_one;      // 通道1数据
    QVector<double> m_channel_two;      // 通道2数据
    QVector<double> m_win;              // 窗
    bool m_win_flag;                    // 是否加窗标识
};

// 数据存储任务类
class DataStore : public QObject
{
    Q_OBJECT
public:
    explicit DataStore(QObject *parent = nullptr);

public slots:
    void slot_store_data(double cn_one, double cn_two);      // 存储数据
    void slot_conn_db(bool ntf);                             // 连接数据库
signals:

public:
    qmysql m_sql;                                            // mysql实例
};


#endif // MYTHREAD_H
