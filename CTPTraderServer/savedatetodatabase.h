#ifndef SAVEDATETODATABASE_H
#define SAVEDATETODATABASE_H

#include "mydatabase.h"
#include "publicstruct.h"
#include <QDateTime>

class SaveDateToDataBase
{
public:
    SaveDateToDataBase() = delete;
    SaveDateToDataBase(QString databaseName);

    void addBarData(QString table_name, BarData& data);
    void insertBarTable(QString symbol, BarData& data);
    void createBarTable(QString);

    void addTickData(QString table_name, std::shared_ptr<QuoteInfo> &quote);
    bool insertTickTable(QString table_name, std::shared_ptr<QuoteInfo> &quote);

    bool addTickData(QString table_name, QList<std::shared_ptr<QuoteInfo> > &quote_list);
    bool insertTickTable(QString table_name, QList<std::shared_ptr<QuoteInfo> > &quote_list);
    bool createTickTable(QString);

private:
    MyDataBase* m_mydatabase;
    QString m_databaseName = "";


//    QSqlDatabase db;    del

//    QSet<QString> table_set;  del
};

#endif // SAVEDATETODATABASE_H
