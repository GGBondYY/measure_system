#include "qmysql.h"
#include <QDebug>

qmysql::qmysql(QObject *parent)
    : QObject{parent}
{
    this->m_conn_name = "";
}

/*
*	@brief:  连接数据库
*	@param:  conn_name:数据库实例名
*	@return: bool 成功/失败 标志
*/
bool qmysql::connect_db(QString conn_name){

    qDebug() << conn_name + "connect_db thread_id: " << QThread::currentThread();
    this->m_conn_name = conn_name;
    // 通过drivers 得到可以使用的数据库驱动名字的集合
    QStringList list = QSqlDatabase::drivers();
    qDebug() << list;
    // 添加一个数据库实例
    this->db = QSqlDatabase::addDatabase("QMYSQL", this->m_conn_name);
    // 为数据库实例设置相关信息
    this->db.setHostName("localhost");      // 主机名 127.0.0.1
    this->db.setPort(3306);                 // mysql端口
    this->db.setUserName("root");           // 用户名
    this->db.setPassword("147258369");      // 数据库密码
    this->db.setDatabaseName("measure");    // 数据库名


    // 连接数据库服务器

    if(!this->db.open()){
        // QMessageBox::warning(this, this->db.lastError().text(), "数据库连接失败");
        qDebug() << "失败原因: " << this->db.lastError().text();
        return false;
    }
    else{
        // QMessageBox::information(this, "success", "measure数据库连接成功");
        qDebug() << "连接成功!";
        return true;
    }
    return false;
}

/*
*	@brief:  关闭数据库连接
*	@param:
*	@return: bool 成功/失败 标志
*/
bool qmysql::close_db()
{
    if(this->if_on_connect()){
        this->db.close();
        // qDebug() << "数据库close成功";
        this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->m_conn_name);
    }


    // qDebug() << "removeDatabase";

    return true;
}

/*
*	@brief:  执行sql语句
*	@param:  sql语句
*	@return: bool 成功/失败 标志
*/
bool qmysql::sql_query(QString sql)
{
    // qDebug() << "sql_query thread_id: " << QThread::currentThread();
    // 在主线程创建的数据库不能在子线程进行使用  需要指定数据库实例
    QSqlQuery query(this->db);
    // 开启一个事务
    this->db.transaction();
    bool flag = false;
    flag = query.exec(sql);

    if(flag){
        // 成功 提交事务
        this->db.commit();
        // qDebug() << "事务提交成功";
        return true;
    }
    else{
        // 失败 回滚事务
        this->db.rollback();
        // qDebug() << "事务提交失败";
        return false;
    }

    return true;
}

/*
*	@brief:  判断数据库是否连接
*	@param:
*	@return: bool 连接/关闭 标志
*/
bool qmysql::if_on_connect()
{
    if(this->db.isOpen()){
        return true;
    }
    return false;
}

/*
*	@brief:  获取数据库错误信息
*	@param:
*	@return: QString 错误信息
*/
QString qmysql::get_error()
{
    return this->db.lastError().text();
}
