#ifndef QMYSQL_H
#define QMYSQL_H

#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMutex>
#include <QSqlQuery>
#include <QThread>
class qmysql : public QObject
{
    Q_OBJECT
public:
    explicit qmysql(QObject *parent = nullptr);

public:
    bool connect_db(QString conn_name);                  // 连接数据库
    bool close_db();                                     // 关闭数据库连接
    bool sql_query(QString sql);                         // 执行sql语句
    bool if_on_connect();                                // 判断数据库是否连接
    QString get_error();                                 // 获取数据库错误信息

signals:

private:
    QSqlDatabase db;                    // 数据库连接实例
    QString m_conn_name;                // 数据库连接名称
};

#endif // QMYSQL_H
