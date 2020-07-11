#include "savedatetodatabase.h"

SaveDateToDataBase::SaveDateToDataBase(QString databaseName)
{
    m_databaseName = databaseName;
    m_mydatabase = new MyDataBase("localhost", databaseName);
}

void SaveDateToDataBase::addBarData(QString table_name, BarData &data)
{
    if(m_mydatabase->IsExist(table_name))
    {
        this->insertBarTable(table_name, data);
    }
    else{
        this->createBarTable(table_name);
        this->insertBarTable(table_name, data);
    }
}

void SaveDateToDataBase::insertBarTable(QString table_name, BarData &data)
{
    QString date = data.tradingDay;
    QString time = data.begin_time;
    QDate date_d = QDate::fromString(date,"yyyyMMdd");
    QTime time_d = QTime::fromString(time,"hh:mm:ss");
    // 计算时间戳
    int time_t = QDateTime(date_d,time_d).toTime_t();

    QMap<QString,QString> table_info;
    table_info.insert("symbol", data.symbol);
    table_info.insert("begin_time", data.begin_time);
    table_info.insert("end_time", data.end_time);
    table_info.insert("timestamp", QString::number(time_t));
    table_info.insert("tradingDay", data.tradingDay);
    table_info.insert("open", QString::number(data.open));
    table_info.insert("high", QString::number(data.high));
    table_info.insert("low", QString::number(data.low));
    table_info.insert("close", QString::number(data.close));
    table_info.insert("begin_volume", QString::number(data.begin_volume));
    table_info.insert("end_volume", QString::number(data.end_volume));
    int ret = m_mydatabase->add(table_name, table_info);
    // qDebug()<< "SaveDateToDataBase::insertBarTable"<< ret;
}

void SaveDateToDataBase::createBarTable(QString table_name)
{
    QMap<QString,QString> table_info;
    table_info.insert("id", "INTEGER PRIMARY KEY AUTOINCREMENT");
    table_info.insert("symbol", "TEXT");
    table_info.insert("begin_time", "TEXT");
    table_info.insert("end_time", "TEXT");
    table_info.insert("timestamp", "INT");
    table_info.insert("tradingDay", "TEXT");
    table_info.insert("open", "DOUBLE");
    table_info.insert("high", "DOUBLE");
    table_info.insert("low", "DOUBLE");
    table_info.insert("close", "DOUBLE");
    table_info.insert("begin_volume", "INT");
    table_info.insert("end_volume", "INT");

    bool ret = m_mydatabase->createTable(table_name, table_info);
    qDebug()<< "SaveDateToDataBase::createBarTable"<< ret;
}

void SaveDateToDataBase::addTickData(QString table_name, std::shared_ptr<QuoteInfo> &quote)
{
    if(m_mydatabase->IsExist(table_name))
    {
        this->insertTickTable(table_name, quote);
    }
    else{
        this->createTickTable(table_name);
        this->insertTickTable(table_name, quote);
    }
}

bool SaveDateToDataBase::insertTickTable(QString table_name, std::shared_ptr<QuoteInfo> &quote)
{
    QString date = quote->tradingDay;
    QString time = quote->time;
    QDate date_d = QDate::fromString(date,"yyyyMMdd");
    QTime time_d = QTime::fromString(time,"hh:mm:ss");
    // 计算时间戳
    int time_t = QDateTime(date_d,time_d).toTime_t();

    QMap<QString,QString> table_info;
    table_info.insert("symbol", quote->symbol);
    table_info.insert("upperLimit", QString::number(quote->upperLimit));
    table_info.insert("lowerLimit", QString::number(quote->lowerLimit));
    table_info.insert("openPrice", QString::number(quote->openPrice));
    table_info.insert("highPrice", QString::number(quote->highPrice));
    table_info.insert("lowPrice", QString::number(quote->lowPrice));
    table_info.insert("lastPrice", QString::number(quote->lastPrice));
    table_info.insert("bidPrice1", QString::number(quote->bidPrice1));
    table_info.insert("bidVolume1", QString::number(quote->bidVolume1));
    table_info.insert("askPrice1", QString::number(quote->askPrice1));
    table_info.insert("askVolume1", QString::number(quote->askVolume1));
    table_info.insert("volume", QString::number(quote->volume));
    table_info.insert("openInterest", QString::number(quote->openInterest));
    table_info.insert("preClosePrice", QString::number(quote->preClosePrice));
    table_info.insert("zuojiePrice", QString::number(quote->preSettlementPrice));
    table_info.insert("time", quote->time);
    table_info.insert("tradingDay", quote->tradingDay);
    table_info.insert("timestamp", QString::number(time_t));


    int ret = m_mydatabase->add(table_name, table_info);
    // qDebug()<< "SaveDateToDataBase::insertTickTable"<< ret;
    return ret;
}

bool SaveDateToDataBase::addTickData(QString table_name, QList<std::shared_ptr<QuoteInfo> > &quote_list)
{
    if(m_mydatabase->IsExist(table_name))
    {
        bool ret = this->insertTickTable(table_name, quote_list);
        qDebug()<< "SaveDateToDataBase::addTickData1"<< table_name<< quote_list.size();
        if(!ret)
            qDebug()<< "SaveDateToDataBase::addTickData"<< table_name<< quote_list.size();
        return ret;
    }
    else{
        if(this->createTickTable(table_name)){
            bool ret = this->insertTickTable(table_name, quote_list);
            return ret;
        }
        else
            return false;

    }
}

bool SaveDateToDataBase::insertTickTable(QString table_name, QList<std::shared_ptr<QuoteInfo> > &quote_list)
{
    m_mydatabase->m_db.transaction();                   // 开启事务
    foreach(auto quote, quote_list)
    {
        if(!this->insertTickTable(table_name, quote))
            return false;
    }
    m_mydatabase->m_db.commit();                        // 提交事务
    return true;
}

bool SaveDateToDataBase::createTickTable(QString table_name)
{
    QMap<QString,QString> table_info;
    table_info.insert("id", "INTEGER PRIMARY KEY AUTOINCREMENT");
    table_info.insert("symbol", "TEXT");
    table_info.insert("upperLimit", "DOUBLE");
    table_info.insert("lowerLimit", "DOUBLE");
    table_info.insert("openPrice", "DOUBLE");
    table_info.insert("highPrice", "DOUBLE");
    table_info.insert("lowPrice", "DOUBLE");
    table_info.insert("lastPrice", "DOUBLE");
    table_info.insert("bidPrice1", "DOUBLE");
    table_info.insert("bidVolume1", "INT");
    table_info.insert("askPrice1", "DOUBLE");
    table_info.insert("askVolume1", "INT");
    table_info.insert("volume", "INT");
    table_info.insert("openInterest", "INT");
    table_info.insert("preClosePrice", "DOUBLE");
    table_info.insert("zuojiePrice", "DOUBLE");
    table_info.insert("time", "TEXT");
    table_info.insert("tradingDay", "TEXT");
    table_info.insert("timestamp", "INT");

    bool ret = m_mydatabase->createTable(table_name, table_info);
    qDebug()<< "SaveDateToDataBase::createBarTable"<< ret;
    return ret;
}
