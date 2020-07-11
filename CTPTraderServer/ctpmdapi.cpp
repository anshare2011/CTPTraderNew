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
    int Result = MdApi->ReqUserLogin(&loginField, requestID++);
    qDebug()<< QString::fromLocal8Bit("MD行情登录")<< Result;
}

void CtpMdApi::ctpMdLogout()
{
    CThostFtdcUserLogoutField logoutField = { 0 };
    int Result = MdApi->ReqUserLogout(&logoutField, requestID++);
    qDebug()<< QString::fromLocal8Bit("MD行情退出")<< Result;
}

void CtpMdApi::ctpMdSubscribeInstrument(QString instrument_name)
{
    qDebug()<< "CtpMdApi::ctpMdSubscribe"<< instrument_name;
    char id[15] = { 0 };
    strncpy_s(id, instrument_name.toStdString().c_str(), sizeof(id));
    char* insts[] = { id };
    qDebug()<< insts<< sizeof(insts);
    int Result = MdApi->SubscribeMarketData(insts, 1);
    qDebug()<<"SubscribeMarketData ret<<" <<  ((Result == 0) ? "success" : "failed");

}

void CtpMdApi::ctpMdUnsubscribeInstrument(QString instrument_name)
{
    qDebug()<< "CtpMdApi::ctpMdUnsubscribeInstrument"<< instrument_name;
    char id[15] = { 0 };
    strncpy_s(id, instrument_name.toStdString().c_str(), sizeof(id));
    char* insts[] = { id };
    int Result = MdApi->UnSubscribeMarketData(insts, 1);
    qDebug()<<"unsubscribeInstrument ret<<" <<  ((Result == 0) ? "success" : "failed");
}

//Spi
void CtpMdApi::OnFrontConnected()
{
    qDebug()<< "CtpMdApi::OnFrontConnected sucess";
    this->ctpMdLogin();
}

void CtpMdApi::OnFrontDisconnected(int nReason)
{
    qDebug()<< "CtpMdApi::OnFrontDisconnected, nReason:"<< nReason;
    is_md_login = false;
}

void CtpMdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(0 == pRspInfo->ErrorID)
    {
        qDebug() << QString::fromLocal8Bit("MD行情登录成功");
        is_md_login = true;
        emit sendMdConnect();
    }
    else
        qDebug() << QString::fromLocal8Bit("MD行情登录失败");
}

void CtpMdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (0 == pRspInfo->ErrorID)
    {
        qDebug() << QString::fromLocal8Bit("MD成功退出");
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

    emit sendQuoteInformation(quote);
}

void CtpMdApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() << "CtpMdApi::OnRspError--->>> "<< __FUNCTION__ << endl;
}
