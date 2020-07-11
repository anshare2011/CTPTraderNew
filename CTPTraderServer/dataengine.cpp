#include "dataengine.h"

//DataEngine::DataEngine(QObject *parent) : QObject(parent)
//{

//}

DataEngine::DataEngine(QString exchange_name)
{
    this->exchange_name = exchange_name;
}

// 接收到行情后数据分发，一个标的 一个OneSymbolKlineData*类， 从OneSymbolKlineData*中存到数据库
void DataEngine::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    // qDebug()<< "DataEngine::receiveQuoteInformation"<< quote->symbol<< quote->time;
    // qDebug()<<quote->time<< quote->symbol<< quote->askPrice1<<quote->bidPrice1<< quote->exchangeid;
    QString symbol = quote->symbol;
    if(all_symbol_data.contains(symbol)){
        OneSymbolKlineData* one_data = all_symbol_data[symbol];
        one_data->updateQuoteInformation(quote);
    }
    else{
        OneSymbolKlineData* one_data = new OneSymbolKlineData(symbol, this->exchange_name);
        one_data->updateQuoteInformation(quote);
        all_symbol_data.insert(symbol, one_data);
    }
}
