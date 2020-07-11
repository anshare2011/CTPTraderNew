#include "ctpmdapi.h"

CtpMdApi::CtpMdApi(QObject *parent) : QObject(parent)
{

}

bool CtpMdApi::getIsMdConnect()
{
    return is_md_login;
}

bool CtpMdApi::getIsMdLogout()
{
    return is_md_logout;
}

void CtpMdApi::ctpMdConnectToExchange(QString mdaddress)
{
    QString con_path = "conn_file/md/";   //md不需要保存在多个账户下
    QDir temp;
    if (!temp.exists(con_path))
        temp.mkpath(con_path);

    char frontaddress[512] = { 0 };
    strncpy_s(frontaddress, mdaddress.toStdString().c_str(), sizeof(frontaddress));

    MdApi = CThostFtdcMdApi::CreateFtdcMdApi(con_path.toStdString().c_str());
    MdApi->RegisterSpi(this);
    MdApi->RegisterFront(frontaddress);
    MdApi->Init();
}

void CtpMdApi::ctpMdRelease()
{
    if (MdApi != nullptr)
    {
        MdApi->RegisterSpi(nullptr);
        MdApi->Release();
        MdApi = nullptr;
    }
}

void CtpMdApi::ctpMdLogin()
{
    CThostFtdcReqUserLoginField loginField = { 0 };
    int Result = MdApi->ReqUserLogin(&loginField, request_id++);
    qDebug()<< QString::fromLocal8Bit("MD行情登录")<< ((Result == 0) ? "success" : "failed");
}

void CtpMdApi::ctpMdLogout()
{
    CThostFtdcUserLogoutField logoutField = { 0 };
    int Result = MdApi->ReqUserLogout(&logoutField, request_id++);
    qDebug()<< QString::fromLocal8Bit("MD行情退出")<< Result;
}

// inside 为true为内部订阅，否则为用户主动订阅
void CtpMdApi::ctpMdSubscribeInstrument(QString instrument_name, bool inside)
{
    if(!is_md_login)
        return;

//    qDebug()<< QString::fromLocal8Bit("订阅合约:")<< instrument_name;
    char id[24] = { 0 };
    strncpy_s(id, instrument_name.toStdString().c_str(), sizeof(id));
    char* insts[] = { id };
    int Result = MdApi->SubscribeMarketData(insts, 1);
//    qDebug()<< QString::fromLocal8Bit("订阅合约:")<<  instrument_name<< ((Result == 0) ? "success" : "failed");

}

void CtpMdApi::ctpMdUnsubscribeInstrument(QString instrument_name)
{
    if(!is_md_login)
        return;

//    qDebug()<< QString::fromLocal8Bit("取消订阅合约:")<< instrument_name;
    char id[16] = { 0 };
    strncpy_s(id, instrument_name.toStdString().c_str(), sizeof(id));
    char* insts[] = { id };
    int Result = MdApi->UnSubscribeMarketData(insts, 1);
//    qDebug()<< QString::fromLocal8Bit("取消订阅合约:")<<  instrument_name<<  ((Result == 0) ? "success" : "failed");
}

void CtpMdApi::ctpMdGetMainContract(QMap<QString, QList<QString> > &id_instrument_list)
{
    for(auto iter = id_instrument_list.begin(); iter!= id_instrument_list.end(); ++iter)
    {
        QList<QString>& temp_list = iter.value();
        for(int i = 0; i<temp_list.size(); ++i)
        {
            this->ctpMdSubscribeInstrument(temp_list.at(i), true);
        }
    }
}

//Spi
void CtpMdApi::OnFrontConnected()
{
    qDebug()<< QString::fromLocal8Bit("行情连接成功");
    this->ctpMdLogin();
}

void CtpMdApi::OnFrontDisconnected(int nReason)
{
    qDebug()<< QString::fromLocal8Bit("行情断开连接,原因:")<< nReason;
    is_md_login = false;
}

void CtpMdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(0 == pRspInfo->ErrorID)
    {
        qDebug() << QString::fromLocal8Bit("MD行情返回登录成功");
        is_md_login = true;
        emit sendMdConnect();
    }
    else
        qDebug() << QString::fromLocal8Bit("MD行情返回登录失败");
}

void CtpMdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (0 == pRspInfo->ErrorID)
    {
        qDebug() << QString::fromLocal8Bit("MD行情成功退出");
        is_md_logout = true;
        emit sendMdDisconnect();
    }
}

void CtpMdApi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
//    qDebug()<< "CtpMdApi::OnRtnDepthMarketData"<< pDepthMarketData->InstrumentID  << pDepthMarketData->UpdateTime
//            << pDepthMarketData->UpperLimitPrice<< pDepthMarketData->LowerLimitPrice
//            << pDepthMarketData->OpenPrice<< pDepthMarketData->LastPrice
//            << pDepthMarketData->BidPrice1<< pDepthMarketData->AskPrice1;

    CThostFtdcDepthMarketDataField *p = pDepthMarketData;

    std::shared_ptr<QuoteInfo> quote(new QuoteInfo);

    quote->symbol = (pDepthMarketData->InstrumentID);
    quote->exchangeid = (pDepthMarketData->ExchangeID);
    quote->upperLimit = (pDepthMarketData->UpperLimitPrice);
    quote->lowerLimit = (pDepthMarketData->LowerLimitPrice);
    quote->openPrice = (pDepthMarketData->OpenPrice);
    quote->highPrice = (pDepthMarketData->HighestPrice);
    quote->lowPrice = (pDepthMarketData->LowestPrice);
    quote->lastPrice = (pDepthMarketData->LastPrice);
    quote->bidPrice1 = (pDepthMarketData->BidPrice1);
    quote->bidVolume1 = (pDepthMarketData->BidVolume1);
    quote->askPrice1 = (pDepthMarketData->AskPrice1);
    quote->askVolume1 = (pDepthMarketData->AskVolume1);
    quote->volume = (pDepthMarketData->Volume);
    quote->openInterest = (pDepthMarketData->OpenInterest);
    quote->preClosePrice = (pDepthMarketData->PreClosePrice);
    quote->preSettlementPrice = (pDepthMarketData->PreSettlementPrice);
    quote->time = (pDepthMarketData->UpdateTime);
    quote->tradingDay = (pDepthMarketData->TradingDay);

//    this->ctpMdUnsubscribeInstrument(quote->symbol);
    emit sendOpenInterest(quote->symbol, quote->openInterest, quote->volume);
    emit sendQuoteInformation(quote);
}

void CtpMdApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() << "CtpMdApi::OnRspError--->>> "<< __FUNCTION__ << endl;
}
