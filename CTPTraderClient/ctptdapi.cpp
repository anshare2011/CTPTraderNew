#include "ctptdapi.h"

CtpTdApi::CtpTdApi(QObject *parent) : QObject(parent)
{

}

bool CtpTdApi::getIsTdConnect()
{
    return is_td_connect;
}

bool CtpTdApi::getIsTdLogout()
{
    return is_td_logout;
}

void CtpTdApi::ctpTdConnectToExchange(QString tdAddress, QString userid, QString password, QString brokerid, QString AppID, QString AuthCode, QString UserProductInfo)
{
    // 保存基本信息
    strncpy_s(frontaddress, tdAddress.toStdString().c_str(), sizeof(frontaddress));
    strncpy_s(m_BrokerID, brokerid.toStdString().c_str(), sizeof(m_BrokerID));
    strncpy_s(m_UserID, userid.toStdString().c_str(), sizeof(m_UserID));
    strncpy_s(m_PassWord, password.toStdString().c_str(), sizeof(m_PassWord));
    strncpy_s(m_AppID, AppID.toStdString().c_str(), sizeof(m_AppID));
    strncpy_s(m_AuthCode, AuthCode.toStdString().c_str(), sizeof(m_AuthCode));
    strncpy_s(m_UserProductInfo, UserProductInfo.toStdString().c_str(), sizeof(m_UserProductInfo));

    QString con_path = "conn_file/" + userid + "/td/";
    QDir temp;
    if (!temp.exists(con_path))
    {
        bool n = temp.mkpath(con_path);         //Creates the directory path dirPath.
    }

    TdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(con_path.toStdString().c_str());
    TdApi->RegisterSpi(this);

    // 订阅共有流、私有流
    TdApi->SubscribePublicTopic(THOST_TERT_RESTART);
    TdApi->SubscribePrivateTopic(THOST_TERT_RESTART);

    // 注册并连接前置机
    TdApi->RegisterFront(frontaddress);
    TdApi->Init();
}

void CtpTdApi::ctpTdRelease()
{
    if (TdApi != nullptr)
    {
        TdApi->RegisterSpi(nullptr);
        TdApi->Release();
        TdApi = nullptr;
    }
}

void CtpTdApi::ctpTdQuery()
{
    //查询合约失败则一直查
    if (!is_qryinstrument)
    {
        ReqQryInstrument();
    }

    query_count += 1;
    if (query_count %2 ==1)
    {
        ReqQryTradingAccount();
    }
    else
    {
        ReqQryInvestorPosition();
    }
}

QString CtpTdApi::ctpTdSendLimitOrder(QString exchangeid, QString instrument, double price, int volume, char direction, char offset, char tradeway = '0')
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    // 合约代码
    strncpy_s(req.InstrumentID, instrument.toStdString().c_str(), sizeof(req.InstrumentID));
    // 报单引用
    sprintf(ORDER_REF, "%d", request_id);
    strcpy(req.OrderRef, ORDER_REF);
    // 报单价格条件: 限价
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    // 价格
    req.LimitPrice = price;
    // 数量
    req.VolumeTotalOriginal = volume;
    // 方向
    req.Direction = direction;
    // 开平
    req.CombOffsetFlag[0] = offset;
    // 交易所
    strncpy_s(req.ExchangeID, exchangeid.toStdString().c_str(), sizeof(req.ExchangeID));

    return this->ctpTdSendOrder(req);
}

QString CtpTdApi::ctpTdSendMarketOrder(QString exchangeid, QString instrumentid, double volume, char direction, char offset, char tradeway)
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    // 合约代码
    strncpy_s(req.InstrumentID, instrumentid.toStdString().c_str(), sizeof(req.InstrumentID));
    // 报单引用
    sprintf(ORDER_REF, "%d", request_id);
    strcpy(req.OrderRef, ORDER_REF);
    // 报单价格条件: 市价
    req.OrderPriceType = THOST_FTDC_OPT_AskPrice1PlusOneTicks;
    // 数量
    req.VolumeTotalOriginal = volume;
    // 方向
    req.Direction = direction;
    // 开平
    req.CombOffsetFlag[0] = offset;
    // 交易所
    strncpy_s(req.ExchangeID, exchangeid.toStdString().c_str(), sizeof(req.ExchangeID));

    return this->ctpTdSendOrder(req);
}

int CtpTdApi::ctpTdCancelOrder(QString instrumentID, QString exchangeID, QString orderID, int frontID, int sessionID)
{
    //设置撤单信息
    CThostFtdcInputOrderActionField orderField = { 0 };
    strncpy_s(orderField.InstrumentID, instrumentID.toStdString().c_str(), sizeof(orderField.InstrumentID));
    strncpy_s(orderField.ExchangeID, exchangeID.toStdString().c_str(), sizeof(orderField.ExchangeID));
    strncpy_s(orderField.OrderRef, orderID.toStdString().c_str(), sizeof(orderField.OrderRef));
    orderField.FrontID = frontID;
    orderField.SessionID = sessionID;
    //删除报单
    orderField.ActionFlag = THOST_FTDC_AF_Delete;
    strncpy_s(orderField.BrokerID, m_BrokerID, sizeof(orderField.BrokerID));
    strncpy_s(orderField.InvestorID, m_UserID, sizeof(orderField.InvestorID));

    int iResult = TdApi->ReqOrderAction(&orderField, ++request_id);
    qDebug() << m_UserID<< QString::fromLocal8Bit("--->>> 撤单录入请求: ") << ((iResult == 0) ? "sucess" : "failed");
    return iResult;
}

// 认证
void CtpTdApi::ReqAuthenticate()
{
    CThostFtdcReqAuthenticateField  pReqAuthenticateField;
    memset(&pReqAuthenticateField, 0, sizeof(CThostFtdcReqAuthenticateField));
    strncpy_s(pReqAuthenticateField.BrokerID, m_BrokerID, sizeof(pReqAuthenticateField.BrokerID));
    strncpy_s(pReqAuthenticateField.UserID, m_UserID, sizeof(pReqAuthenticateField.UserID));
    strncpy_s(pReqAuthenticateField.AppID, m_AppID, sizeof(pReqAuthenticateField.AppID));
    strncpy_s(pReqAuthenticateField.AuthCode, m_AuthCode, sizeof(pReqAuthenticateField.AuthCode));
    int iResult = TdApi->ReqAuthenticate(&pReqAuthenticateField, ++request_id);
    qDebug() << m_UserID<< QString::fromLocal8Bit("发送交易认证请求: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 登录
void CtpTdApi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.UserID, m_UserID, sizeof(req.UserID));
    strncpy_s(req.Password, m_PassWord, sizeof(req.Password));
    int iResult = TdApi->ReqUserLogin(&req, ++request_id);
    qDebug()<< m_UserID<< QString::fromLocal8Bit("发送登录请求: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 退出
void CtpTdApi::ReqUserLogout()
{
    CThostFtdcUserLogoutField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.UserID, m_UserID, sizeof(req.UserID));
    int iResult = TdApi->ReqUserLogout(&req, ++request_id);
    qDebug()<< m_UserID<< QString::fromLocal8Bit("发送退出请求: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 投资者确认
void CtpTdApi::ReqSettlementInfoConfirm()
{
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.InvestorID, m_UserID, sizeof(req.InvestorID));
    int iResult = TdApi->ReqSettlementInfoConfirm(&req, ++request_id);
    qDebug()<< m_UserID<< QString::fromLocal8Bit("投资者结算结果确认: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 查询合约基本信息
void CtpTdApi::ReqQryInstrument()
{
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.ExchangeID, "", sizeof(req.ExchangeID));
    int iResult = TdApi->ReqQryInstrument(&req, ++request_id);
    if (iResult == 0) is_qryinstrument = true;
    qDebug()<< m_UserID<< QString::fromLocal8Bit("请求查询合约信息: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 请求查询合约手续费
void CtpTdApi::ReqQryInstrumentCommissionRate(QString instrument)
{
    CThostFtdcQryInstrumentCommissionRateField req = { 0 };
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.InvestorID, m_UserID, sizeof(req.InvestorID));
    strncpy_s(req.InstrumentID, instrument.toStdString().c_str(), sizeof(req.InstrumentID));
    strncpy_s(req.ExchangeID, "SHFE", sizeof(req.ExchangeID));

    //    strncpy_s(req.InstrumentID, instrument.toStdString().c_str(), sizeof(req.InstrumentID));
    //    strncpy_s(req.ExchangeID, "SHFE", sizeof(req.ExchangeID));
    int iResult = TdApi->ReqQryInstrumentCommissionRate(&req, ++request_id);
    qDebug()<< m_UserID<< QString::fromLocal8Bit("--->>> 请求查询合约手续费: ") <<  ((iResult == 0) ? "sucess" : "failed");
}

// 请求查询资金账户
void CtpTdApi::ReqQryTradingAccount()
{
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.InvestorID, m_UserID, sizeof(req.InvestorID));
    int iResult = TdApi->ReqQryTradingAccount(&req, ++request_id);
    //    qDebug() << m_UserID<< QString::fromLocal8Bit("--->>> 请求查询资金账户: ") << ((iResult == 0) ? "success" : "failed");
}

// 请求查询投资者持仓
void CtpTdApi::ReqQryInvestorPosition()
{
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.BrokerID, m_BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.InvestorID, m_UserID, sizeof(req.InvestorID));
    //    strcpy(req.InstrumentID, INSTRUMENT_ID);
    int iResult = TdApi->ReqQryInvestorPosition(&req, ++request_id);
    //    qDebug() << m_UserID<<QString::fromLocal8Bit("--->>> 请求查询投资者持仓: ") << ((iResult == 0) ? "success" : "failed");
}

bool CtpTdApi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    return (pRspInfo && (pRspInfo->ErrorID != 0));
}

bool CtpTdApi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == FRONT_ID) &&
            (pOrder->SessionID == SESSION_ID) &&
            (strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CtpTdApi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

// 改名字
QString CtpTdApi::ctpTdSendOrder(CThostFtdcInputOrderField &orderField)
{
    strncpy_s(orderField.BrokerID, m_BrokerID, sizeof(orderField.BrokerID));
    strncpy_s(orderField.InvestorID, m_UserID, sizeof(orderField.InvestorID));
    // 组合投机套保标志
    orderField.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    // 有效期类型: 当日有效
    orderField.TimeCondition = THOST_FTDC_TC_GFD;
    // 成交量类型: 任何数量
    orderField.VolumeCondition = THOST_FTDC_VC_AV;
    // 最小成交量: 1
    orderField.MinVolume = 1;
    // 触发条件: 立即
    orderField.ContingentCondition = THOST_FTDC_CC_Immediately;
    // 非强平 '0'
    orderField.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    // 自动挂起标志: 否
    orderField.IsAutoSuspend = 0;
    orderField.UserForceClose = 0;

    int iResult = TdApi->ReqOrderInsert(&orderField, ++request_id);
    qDebug() << m_UserID << QString::fromLocal8Bit("--->>> 报单录入请求: ") << ((iResult == 0) ? "sucess" : "failed")
             << QString::fromLocal8Bit("报单编号: ")<< orderField.OrderRef;
    return orderField.OrderRef;
}

void CtpTdApi::myDelayMSecond(unsigned int msec)
{
    //定义一个新的事件循环
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));       // 创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();                                         // 事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

// Spi
// 连接第一步开始认证
void CtpTdApi::OnFrontConnected()
{
    is_td_connect = true;
    qDebug() << m_UserID<< QString::fromLocal8Bit("交易连接成功");
    ReqAuthenticate();
}

void CtpTdApi::OnFrontDisconnected(int nReason)
{
    is_td_connect = false;
    qDebug() << m_UserID<< QString::fromLocal8Bit("断开交易连接");
}

void CtpTdApi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && bIsLast)
    {
        qDebug()<< m_UserID<<QString::fromLocal8Bit("交易认证成功");
        ReqUserLogin();
    }
    else{
        qDebug()<< m_UserID<<QString::fromLocal8Bit("交易认证失败");
    }
}

void CtpTdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && bIsLast)
    {
        emit isTdConnect(QString(m_UserID));

        CThostFtdcRspUserLoginField p = *pRspUserLogin;

        qDebug() << m_UserID<< QString::fromLocal8Bit("交易登录成功");
        // 保存账户信息
        FRONT_ID = pRspUserLogin->FrontID;
        SESSION_ID = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        sprintf(ORDER_REF, "%d", iNextOrderRef);
        // 获取当前交易日
        qDebug() << QString::fromLocal8Bit("获取当前交易日 = ") << TdApi->GetTradingDay();
        //  确认结算结果
        ReqSettlementInfoConfirm();
    }
    else{
        qDebug() << m_UserID<< QString::fromLocal8Bit("交易登录失败");
    }
}

void CtpTdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pUserLogout)
    {
        is_td_logout = true;
        qDebug() << m_UserID<< QString::fromLocal8Bit("交易接口成功退出");
        emit isTdDisconnect(QString(m_UserID));
    }
}

void CtpTdApi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm)
    {
        // 结算单确认以后才可交易
        is_td_tradable = true;

        // 结算单确认成功(所有qDebug的地方输出到日志监控模块)
        qDebug() << m_UserID<< QString::fromLocal8Bit("投资者确认成功");

        //获取所有合约信息
        ReqQryInstrument();
    }
}

void CtpTdApi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(IsErrorRspInfo(pRspInfo))
    {
        QString theme = QString::fromLocal8Bit("报单插入失败");
        QString content = QString::fromLocal8Bit(pRspInfo->ErrorMsg);
        emit sendTdInformation(theme, content);
    }
    else{
        QString theme = QString::fromLocal8Bit("报单成功");
        QString content = QString(pInputOrder->OrderRef);
        emit sendTdInformation(theme, content);
    }
}

void CtpTdApi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(IsErrorRspInfo(pRspInfo))
    {
        qDebug() << QString::fromLocal8Bit("报单撤单失败");
        QString symbol = QString(pInputOrderAction->InstrumentID);
        QString orderRef = QString(pInputOrderAction->OrderRef);
        QString limitPrice = QString::number(pInputOrderAction->LimitPrice);
    }
    else{
        qDebug() << QString::fromLocal8Bit("撤单成功");
    }
}

// 订单回报
void CtpTdApi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    std::shared_ptr<OrderInfo> order(new OrderInfo);
    order->direction = (pOrder->Direction);
    order->offset = (pOrder->CombOffsetFlag[0]);
    order->tradeway = 'a';                          // 下单方式由本地处理
    order->status = (pOrder->OrderStatus);

    order->totalVolume = (pOrder->VolumeTotalOriginal);
    order->tradeVolume = (pOrder->VolumeTraded);
    order->frontID = (pOrder->FrontID);
    order->sessionID = (pOrder->SessionID);
    order->price = (pOrder->LimitPrice);

    order->userId = m_UserID;
    order->symbol = (pOrder->InstrumentID);
    order->exchange = (pOrder->ExchangeID);
    order->orderID = (pOrder->OrderRef);
    order->orderTime = (pOrder->InsertTime);
    order->cancelTime = (pOrder->CancelTime);
    order->gatewayName = ("CTP");

    sendOrderInformation(order);
}

// 成交回报
void CtpTdApi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    std::shared_ptr<TradeInfo> trade(new TradeInfo);
    trade->direction = (pTrade->Direction);
    trade->offset = (pTrade->OffsetFlag);
    trade->tradeway = 'a';                          // 下单方式由本地处理
    trade->volume = (pTrade->Volume);
    trade->price = (pTrade->Price);

    trade->userId = m_UserID;
    trade->symbol = (pTrade->InstrumentID);
    trade->exchange = (pTrade->ExchangeID);
    trade->tradeID = (pTrade->TradeID);
    trade->orderID = (pTrade->OrderRef);
    trade->tradeTime = (pTrade->TradeTime);
    trade->gatewayName = ("CTP");

    sendTradeInformation(trade);
}

// 持仓回报
void CtpTdApi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition){
        std::shared_ptr<PositionInfo> position(new PositionInfo);
        position->direction = pInvestorPosition->PosiDirection;
        position->position = pInvestorPosition->Position;
        position->ydPosition = pInvestorPosition->YdPosition;
        position->tdPosition = position->position - position->ydPosition;
        position->openCost = pInvestorPosition->OpenCost;

        position->userId = pInvestorPosition->InvestorID;
        position->symbol = pInvestorPosition->InstrumentID;
        position->direction = pInvestorPosition->PosiDirection;
        position->gatewayName = QString("CTP");
        position->exchangeId = pInvestorPosition->ExchangeID;
        position->SettlementID = QString::number(pInvestorPosition->SettlementID);

        position->multiplier = 0;         // 由传出后处理更新
        position->winLoss = 0;
        position->averagePrice = 0;
        position->lastPrice = 0;

        emit sendPositionInformation(position);
    }
    if(bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        std::shared_ptr<PositionInfo> position(new PositionInfo);
        position->is_end = true;
        emit sendPositionInformation(position);
    }
}

// 账户查询回报
void CtpTdApi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pTradingAccount){
        std::shared_ptr<AccountInfo> account(new AccountInfo);

        account->preBalance = pTradingAccount->PreBalance;
        account->balance = pTradingAccount->Balance;
        account->available = pTradingAccount->Available;
        account->commission = pTradingAccount->Commission;
        account->margin = pTradingAccount->CurrMargin;
        account->close_profit = pTradingAccount->CloseProfit;
        account->position_profit = pTradingAccount->PositionProfit;

        account->id = pTradingAccount->AccountID;
        account->gatewayName = ("CTP");

        emit sendAccountInformation(account);
    }
    else{

    }

    // 查询账户后自动查询持仓
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ReqQryInvestorPosition();
    }
}

void CtpTdApi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CThostFtdcInstrumentCommissionRateField *temp = pInstrumentCommissionRate;
    if (!IsErrorRspInfo(pRspInfo) && pInstrumentCommissionRate)
    {
        CommissionRate rate_info;
        rate_info.openRatio = pInstrumentCommissionRate->OpenRatioByMoney;
        rate_info.closeRatio = pInstrumentCommissionRate->CloseRatioByMoney;
        rate_info.closeTodayRatio = pInstrumentCommissionRate->CloseTodayRatioByMoney;
        rate_info.InstrumentID = pInstrumentCommissionRate->InstrumentID;
        emit sendInstrumentCommissionRate(rate_info);
    }
}

void CtpTdApi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pInstrument && pInstrument->ProductClass == THOST_FTDC_PC_Futures)
    {
        CThostFtdcInstrumentField *temp = pInstrument;
        InstrumentInfo instrument_info;
        instrument_info.tradable = pInstrument->IsTrading;
        instrument_info.is_end = false;
        instrument_info.multiplier = pInstrument->VolumeMultiple;
        instrument_info.marginRate = pInstrument->LongMarginRatio;
        instrument_info.minimumUnit = pInstrument->PriceTick;
        instrument_info.symbol = pInstrument->InstrumentID;
        instrument_info.name = QString::fromLocal8Bit(pInstrument->InstrumentName);
        instrument_info.exchangeId = QString(pInstrument->ExchangeID);
        instrument_info.ProductID = pInstrument->ProductID;
        instrument_info.deadline = QDate::fromString(QString(pInstrument->ExpireDate), "yyyyMMdd");

        emit sendInstrumentInformation(instrument_info);
    }

    if (!IsErrorRspInfo(pRspInfo) && pInstrument && pInstrument->ProductClass == THOST_FTDC_PC_Options)
    {
        CThostFtdcInstrumentField *temp = pInstrument;
        //        qDebug()<< "CtpTdApi::OnRspQryInstrument  THOST_FTDC_PC_Options"<< pInstrument->InstrumentID
        //                << QString::fromLocal8Bit(pInstrument->InstrumentName)<< QString(pInstrument->ExchangeID)
        //                << pInstrument->ProductID<< pInstrument->PriceTick<< pInstrument->IsTrading<< pInstrument->OptionsType;

        OptionInfo option_info;
        option_info.isTrading = pInstrument->IsTrading;
        option_info.OptionsType = pInstrument->OptionsType;
        option_info.PositionDateType = pInstrument->PositionDateType;
        option_info.PositionType = pInstrument->PositionType;
        option_info.multiplier = pInstrument->VolumeMultiple;
        option_info.PriceTick = pInstrument->PriceTick;
        option_info.StrikePrice = pInstrument->StrikePrice;
        option_info.EndDelivDate = pInstrument->EndDelivDate;
        option_info.symbol = pInstrument->InstrumentID;
        option_info.ProductID = pInstrument->ProductID;
        option_info.exchangeId = pInstrument->ExchangeID;
        option_info.ExpireDate = pInstrument->ExpireDate;
        option_info.UnderlyingSymbol = pInstrument->UnderlyingInstrID;

        emit sendOptionInformation(option_info);
    }
    if (bIsLast)
    {
        InstrumentInfo instrument_info;
        instrument_info.is_end = true;
        emit sendInstrumentInformation(instrument_info);
        // ReqQryTradingAccount();
        // 同时请求会限流
        // this->ReqQryInstrumentCommissionRate("IF2006");
    }
}

