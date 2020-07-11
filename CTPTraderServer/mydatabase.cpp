#include "mydatabase.h"


MyDataBase::MyDataBase(QString HostName, QString DatabaseName)
{
    QString con_path = "DataBase/";      // 简化处理，数据库文件都放在 DataBase 这个文件目录下
    QDir temp;
    if (!temp.exists(con_path))
    {
        bool n = temp.mkpath(con_path);   //Creates the directory path dirPath.
    }

    m_HostName = HostName;
    m_DatabaseName = con_path + DatabaseName;
    //    m_db = QSqlDatabase::addDatabase("QSQLITE", "qsqlite");   // 后面一个是连接名  , "qsqlite"

    if(QSqlDatabase::contains("qsqlite1")){   // qsqlite    qt_sql_default_connection
        m_db = QSqlDatabase::database("qsqlite1");
    }else{
        m_db = QSqlDatabase::addDatabase("QSQLITE", "qsqlite1");
    }

    m_db.setHostName(m_HostName);
    m_db.setDatabaseName(m_DatabaseName);
    m_query = new QSqlQuery(m_db);
    if(!m_db.open())
    {
        qDebug()<<"database open error.";
        m_error = m_db.lastError();
    }
    else
    {
        qDebug()<<"database open successful.";
    }
}

MyDataBase::MyDataBase(QString HostName, QString DatabaseName, QString UserNmae, QString Password)
{
    m_HostName = HostName;
    m_DatabaseName = DatabaseName;
    m_UserNmae = UserNmae;
    m_Password = Password;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setHostName(m_HostName);
    m_db.setDatabaseName(m_DatabaseName);
    m_db.setUserName(m_UserNmae);
    m_db.setPassword(m_Password);
    m_query = new QSqlQuery(m_db);
    if(!m_db.open())
    {
        qDebug()<<"database open error.";
        m_error = m_db.lastError();
    }
}

MyDataBase::~MyDataBase()
{
    QString name;
    {
        name = QSqlDatabase::database().connectionName();
    }//超出作用域，隐含对象QSqlDatabase::database()被删除。
    QSqlDatabase::removeDatabase(name);
    m_db.close();
    delete m_query;
}

QSqlDatabase MyDataBase::returnDB()
{
    return m_db;
}

bool MyDataBase::createTable(QString table_name, QMap<QString, QString> table_data)
{
    //    QSqlQuery query = QSqlQuery::QSqlQuery(m_db);
    QString sql="create table "+table_name+" (";
    for(QMap<QString,QString>::const_iterator i=table_data.constBegin();i!=table_data.constEnd();i++)
    {
        sql += i.key()+' '+i.value();
        if(i!=table_data.constEnd())
            sql+=',';
    }
    sql.chop(1);
    sql+=")";
    qDebug()<<sql;
    m_query->prepare(sql);
    return m_query->exec();
}

bool MyDataBase::dbExec(QString m_sql)
{
    m_query->prepare(m_sql);
    return m_query->exec();
}

void MyDataBase::qstring_to_char(QString str, char * data)
{
    char* ptr;
    QByteArray ba;
    ba = str.toLatin1();
    ptr = ba.data();
    strcpy(data, ptr);
}

bool MyDataBase::dbExec(QString m_sql, QList<TickData> &data_list)
{
    m_query->prepare(m_sql);
    if(m_query->exec())
    {
        while (m_query->next()) {
            TickData data;
            QSqlRecord record = m_query->record();
            qDebug()<< record.value("symbol").toString()<< record.value("begin_time").toString()<< record.value("end_time").toString()
                       << record.value("open").toString()<< record.value("high").toString()<< record.value("low").toString();
            data.begin_volume = record.value("begin_volume").toString().toInt();
            data.end_volume = record.value("end_volume").toString().toInt();
            data.open = record.value("open").toString().toDouble();
            data.high = record.value("high").toString().toDouble();
            data.low = record.value("low").toString().toDouble();
            data.close = record.value("close").toString().toDouble();

            qstring_to_char(record.value("begin_time").toString(), data.begin_time);
            qstring_to_char(record.value("end_time").toString(), data.end_time);
            qstring_to_char(record.value("tradingDay").toString(), data.tradingDay);
            data_list.append(data);
        }
        return 1;
    }
    return 0;
}

bool MyDataBase::add(QString table_name, QMap<QString, QString> data)
{
    QString sql="insert into "+table_name+  "(";
    QString values=" values(";
    for(QMap<QString,QString>::const_iterator i=data.constBegin();i!=data.constEnd();i++)
    {
        sql+=i.key()+", ";
        values+="'"+ i.value()+ "'" +", ";
    }
    sql.chop(2);
    values.chop(2);
    sql+=")";
    values+=")";
    sql+=values;

    m_query->prepare(sql);
    bool ret = m_query->exec();
    if(!ret)
    {
        m_error = m_query->lastError();
        qDebug()<<m_error.driverText()<<QString::fromLocal8Bit("数据插入失败");
    }
    return ret;
}

bool MyDataBase::del(QString table_name, QMap<QString, QString> where)
{
    QString sql = "delete from ";
    sql+= table_name;
    sql+=" where ";
    for(QMap<QString,QString>::const_iterator i=where.constBegin();i!=where.constEnd();i++)
    {
        sql+=i.key()+"=";
        sql+="'"+i.value()+"' ";
    }
    sql.chop(1);
    qDebug()<<sql;
    m_query->prepare(sql);
    return m_query->exec();
}

bool MyDataBase::update(QString table_name, QMap<QString, QString> where, QMap<QString, QString> data)
{
    QString sql="update "+table_name+" set ";
    for(QMap<QString,QString>::const_iterator i=data.constBegin();i!=data.constEnd();i++)
    {
        //        sql+="'"+ i.key()+ "'" +"=";
        //        sql+="'"+ i.value()+ "'" +" , ";
        sql+=i.key() +"=";
        sql+="'"+ i.value()+ "'" + " , ";
    }
    sql.chop(2);
    sql+="where ";
    for(QMap<QString,QString>::const_iterator i=where.constBegin();i!=where.constEnd();i++)
    {
        //        sql+="'"+ i.key()+ "'" +"=";
        //        sql+="'"+ i.value()+ "'" +" , ";
        sql+=i.key() +"=";
        sql+="'"+ i.value()+ "'"+ " and ";
    }
    sql.chop(4);
    qDebug()<<sql;
    m_query->prepare(sql);
    return m_query->exec();
}

bool MyDataBase::find(QString table_name, QList<QString> key, QMap<QString, QString> where, QList<QList<QString> > *row)
{
    QString sql="select ";
    int len=key.size();
    for(int i=0;i<len;i++)
    {
        sql+=key.at(i);
        sql+=",";
    }
    sql.chop(1);
    sql+=" from "+table_name;
    sql+=" where ";
    for(QMap<QString,QString>::const_iterator i=where.constBegin();i!=where.constEnd();i++)
    {
        sql+=i.key()+"="+"'"+i.value()+"'"+" and ";
    }
    sql.chop(4);
    qDebug()<<sql;
    m_query->prepare(sql);
    if(m_query->exec())
    {
        while (m_query->next()) {
            QList<QString> j;
            for(int i=0;i<len;i++)
            {
                j.append(m_query->value(i).toString());
            }
            row->append(j);
        }
        return 1;
    }
    else return 0;
}

bool MyDataBase::find(QString table_name, QList<QString> key, QList<QList<QString> > *row)
{
    QString sql="select ";
    int len=key.size();
    for(int i=0;i<len;i++)
    {
        sql+=key.at(i);
        sql+=",";
    }
    sql.chop(1);
    sql+=" from "+table_name;
    qDebug()<<sql;
    m_query->prepare(sql);
    if(m_query->exec())
    {
        while (m_query->next()) {
            QList<QString> j;
            for(int i=0;i<len;i++)
            {
                j.append(m_query->value(i).toString());
            }
            row->append(j);
        }
        return 1;
    }
    else return 0;
}

QSqlQuery *MyDataBase::find(QString table_name, QList<QString> key)
{
    QString sql="select ";
    int len=key.size();
    for(int i=0;i<len;i++)
    {
        sql+=key.at(i);
        sql+=",";
    }
    sql.chop(1);
    sql+=" from "+table_name;
    m_query->prepare(sql);
    if(m_query->exec())
    {
        return m_query;
    }
    else return false;
}

bool MyDataBase::IsExist(QString tableName)
{
    m_query->exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(tableName));
    if(m_query->next())
    {
        if(m_query->value(0).toInt()==0)
        {
            // 表不存在
            return false;
        }else{
            return true;
        }
    }
}

QString MyDataBase::getError()
{
    return m_error.text();
}
