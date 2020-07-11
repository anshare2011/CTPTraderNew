#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QDir>
#include <QList>
#include "publicstruct.h"

class MyDataBase
{
public:
    MyDataBase() = delete;
    MyDataBase(QString HostName, QString DatabaseName);     //QSQLITE不需要用户名与密码
    MyDataBase(QString HostName, QString DatabaseName, QString UserNmae, QString Password);
    ~MyDataBase();

    QSqlDatabase returnDB();

    // 创建表
    bool createTable(QString table_name,QMap<QString,QString> table_data);     //函数模板
    // 执行数据库语句
    bool dbExec(QString m_sql);
    void qstring_to_char(QString, char*);
    bool dbExec(QString m_sql, QList<TickData>&data_list);
    // 增加（插入）数据
    bool add(QString table_name,QMap<QString,QString> data);
    // 删除一条记录
    bool del(QString table_name,QMap<QString,QString> where);
    // 更新数据
    bool update(QString table_name,QMap<QString,QString> where,QMap<QString,QString> data);
    // 查找
    bool find(QString table_name,QList<QString> key,QMap<QString,QString> where,QList<QList<QString>> *row);    //查找
    bool find(QString table_name,QList<QString> key,QList<QList<QString>> *row);    //查找所有
    QSqlQuery* find(QString table_name,QList<QString> key);
    // QList<QStringList> find(QString table_name);
    // 判断表格是否存在
    bool IsExist(QString tableName);
    QString getError();

//private:
public:
    QString m_HostName;
    QString m_DatabaseName;
    QString m_UserNmae;
    QString m_Password;
    QSqlError m_error;
    QSqlRecord m_rec;
    QSqlQuery * m_query {nullptr};
    QSqlDatabase m_db;
};

#endif // MYDATABASE_H
