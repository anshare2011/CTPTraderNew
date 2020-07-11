#include "dataengine.h"

DataEngine::DataEngine(QObject *parent) : QObject(parent)
{

}

void DataEngine::updataInstrumentInfo(InstrumentInfo &info)
{
    symbol_name[info.symbol] = info.name;
    symbol_exchangeId[info.symbol] = info.exchangeId;
}

void DataEngine::addSubscribeInstrument(QString instrument)
{
    subscribe_instrument.insert(instrument);
}

OneSymbolKlineData *DataEngine::returnOneSymbolKlineData(QString symbol)
{
    OneSymbolKlineData* temp= nullptr;
    if(all_symbol_data.contains(symbol))
        temp = all_symbol_data[symbol];
    return temp;
}

void DataEngine::getKlineDataFromSerrver(QString symbol, KlineType type, int size)
{
    GetDataFromServer *m_getdata = GetDataFromServer::GetInstance();
    TCP_Head head;
    GetKlineData data;
    quint16 total_size = sizeof(TCP_Head) + sizeof(GetKlineData);
    head.wPacketSize = total_size;
    head.wMainCmdID = MDM_GR_KLINE;
    head.wSubCmdID = SUB_GR_KLINE_INFO;

    data.type = 0;
    data.frequence = type;
    data.kline_size = size;
    strncpy_s(data.symbol, symbol.toStdString().c_str(), sizeof(data.symbol));
    QString exchange_id = this->symbol_exchangeId.contains(symbol) ? this->symbol_exchangeId[symbol] :"" ;
    strncpy_s(data.exchange_id, exchange_id.toStdString().c_str(), sizeof(data.exchange_id));

    QByteArray ba;
    ba.resize(total_size);
    memcpy(ba.data() , &head, sizeof(TCP_Head));
    memcpy(ba.data() + sizeof(TCP_Head) , &data, sizeof(GetKlineData));

    m_getdata->sendData(total_size, (void *)ba.data());
}

void DataEngine::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    QString symbol = quote->symbol;

    // subscribe_instrument 是主动订阅的合约，不包含期权
    if(subscribe_instrument.contains(symbol))
        this->updateQuoteAndSendToMainwindow(symbol, quote);
    // 向外(下单板)分发行情

    // 这里保存最新的行情？
    if(symbol_name.contains(symbol))
        quote->symbol_name = symbol_name[symbol];
    if(symbol_exchangeId.contains(symbol))                // 行情里面带交易所，不需要
        quote->exchangeid = symbol_exchangeId[symbol];
    emit sendQuoteInformation(quote);
}

void DataEngine::receiveKlineBarData(QString symbol, KlineType type, QVector<BarData> klinedata)
{
    qDebug()<< "DataEngine::receiveKlineBarData";
    if(all_symbol_data.contains(symbol))
    {
        OneSymbolKlineData* one_data = all_symbol_data[symbol];
        one_data->receiveKlineBarData(type, klinedata);
    }
    else
    {
        OneSymbolKlineData* one_data = new OneSymbolKlineData(symbol);
        one_data->receiveKlineBarData(type, klinedata);
        all_symbol_data.insert(symbol, one_data);
    }
}

// MainWindow::dataengineConnectToMainwindow 里添加的信号与槽
void DataEngine::updateQuoteAndSendToMainwindow(QString symbol, std::shared_ptr<QuoteInfo> quote)
{
    if(all_symbol_data.find(symbol) != all_symbol_data.end())
    {
        OneSymbolKlineData* one_data = all_symbol_data[symbol];
        one_data->updateQuoteInformation(quote);
    }
    else
    {
        OneSymbolKlineData* one_data = new OneSymbolKlineData(symbol);
        one_data->updateQuoteInformation(quote);
        one_data->upperLimitPrice = quote->upperLimit;
        one_data->lowerLimitPrice = quote->lowerLimit;
        all_symbol_data.insert(symbol, one_data);


        // 获取历史数据
        this->getKlineDataFromSerrver(symbol, KlineType::Min1, 400);
        this->getKlineDataFromSerrver(symbol, KlineType::Min3, 200);
        this->getKlineDataFromSerrver(symbol, KlineType::Min5, 200);
        this->getKlineDataFromSerrver(symbol, KlineType::Min15, 200);
    }
    emit sendKlineInformation(symbol, all_symbol_data[symbol]);
}
