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
    strncpy_s(usr_td_info.BrokerID, brokerid.toStdString().c_str(), sizeof(usr_td_info.BrokerID));
    strncpy_s(usr_td_info.UserID, userid.toStdString().c_str(), sizeof(usr_td_info.UserID));
    strncpy_s(usr_td_info.PassWord, password.toStdString().c_str(), sizeof(usr_td_info.PassWord));
    strncpy_s(usr_td_info.AppID, AppID.toStdString().c_str(), sizeof(usr_td_info.AppID));            // 产品名称
    strncpy_s(usr_td_info.AuthCode, AuthCode.toStdString().c_str(), sizeof(usr_td_info.AuthCode));    // 授权编码
    strncpy_s(usr_td_info.UserProductInfo, UserProductInfo.toStdString().c_str(), sizeof(usr_td_info.UserProductInfo));    // 产品信息

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

// 定时查询
void CtpTdApi::ctpTdQuery()
{
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

QString CtpTdApi::ctpTdSendLimitOrder(QString exchangeid, QString instrument, double price, int volume, char direction, char offset, char tradeway)
{
    orderid_sender[iRequestTdID] = tradeway;

    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    // 合约代码
    strncpy_s(req.InstrumentID, instrument.toStdString().c_str(), sizeof(req.InstrumentID));
    // 报单引用
    sprintf(ORDER_REF,"%d",iRequestTdID);
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
    orderid_sender[iRequestTdID] = tradeway;

    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    // 合约代码
    strncpy_s(req.InstrumentID, instrumentid.toStdString().c_str(), sizeof(req.InstrumentID));
    // 报单引用
    sprintf(ORDER_REF,"%d",iRequestTdID);
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

QString CtpTdApi::ctpTdSendOrder(CThostFtdcInputOrderField &orderField)
{
    //    if (!is_td_tradable.load())
    //    {
    //        // 结算单未确认(所有qDebug的地方输出到日志监控模块)
    //        qDebug() << QString::fromLocal8Bit("is_td_tradable load failed,Miss Settlement Confirmation");
    //        return "Error";
    //    }

    strncpy_s(orderField.BrokerID, usr_td_info.BrokerID, sizeof(orderField.BrokerID));
    strncpy_s(orderField.InvestorID, usr_td_info.UserID, sizeof(orderField.InvestorID));
    //    _snprintf_s(orderField.OrderRef, sizeof(orderField.OrderRef), _TRUNCATE, "%012d", ++usr_td_info.maxOrderRef);  //先看上面的可不可以用
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

    int iResult = TdApi->ReqOrderInsert(&orderField, ++iRequestTdID);
    qDebug() << usr_td_info.UserID << QString::fromLocal8Bit("--->>> 报单录入请求: ") << ((iResult == 0) ? "sucess" : "failed");
    return orderField.OrderRef;
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
    strncpy_s(orderField.BrokerID, usr_td_info.BrokerID, sizeof(orderField.BrokerID));
    strncpy_s(orderField.InvestorID, usr_td_info.UserID, sizeof(orderField.InvestorID));

    int iResult = TdApi->ReqOrderAction(&orderField, ++iRequestTdID);
    qDebug() << QString::fromLocal8Bit("--->>> 撤单录入请求: ") << ((iResult == 0) ? "sucess" : "failed");
    return iResult;
}

// 认证
void CtpTdApi::ReqAuthenticate()
{
    //认证码
    CThostFtdcReqAuthenticateField  pReqAuthenticateField;
    memset(&pReqAuthenticateField, 0, sizeof(CThostFtdcReqAuthenticateField));
    strcpy(pReqAuthenticateField.BrokerID, usr_td_info.BrokerID);
    strcpy(pReqAuthenticateField.UserID, usr_td_info.UserID);
    strcpy(pReqAuthenticateField.AppID, usr_td_info.AppID);             // 产品名称
    strcpy(pReqAuthenticateField.AuthCode, usr_td_info.AuthCode);              // 授权编码
    int iResult = TdApi->ReqAuthenticate(&pReqAuthenticateField, ++iRequestTdID);
    qDebug() << QString::fromLocal8Bit("--->>> 发送认证请求: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 请求登录
void CtpTdApi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, usr_td_info.BrokerID);
    strcpy(req.UserID, usr_td_info.UserID);
    strcpy(req.Password, usr_td_info.PassWord);
    int iResult = TdApi->ReqUserLogin(&req, ++iRequestTdID);
    qDebug()<< QString::fromLocal8Bit("--->>> 发送用户登录请求: ") << ((iResult == 0) ? "sucess" : "failed");
    if((iResult == 0))
    {

        // 改，放在OnRspUserLogin中，退出时也发送告知
        //        emit sendUserid(usr_td_info.UserID);
    }
}

// 请求退出
void CtpTdApi::ReqUserLogout()
{
    CThostFtdcUserLogoutField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, usr_td_info.BrokerID);
    strcpy(req.UserID, usr_td_info.UserID);
    int iResult = TdApi->ReqUserLogout(&req, ++iRequestTdID);
    qDebug()<< QString::fromLocal8Bit("--->>> 发送用户退出请求: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 投资者确认
void CtpTdApi::ReqSettlementInfoConfirm()
{
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, usr_td_info.BrokerID);
    strcpy(req.InvestorID, usr_td_info.UserID);
    int iResult = TdApi->ReqSettlementInfoConfirm(&req, ++iRequestTdID);
    qDebug()<< QString::fromLocal8Bit("--->>> 投资者结算结果确认: ") << ((iResult == 0) ? "sucess" : "failed");
}

// 查询合约基本信息
void CtpTdApi::ReqQryInstrument()
{
    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strncpy_s(req.ExchangeID, "", sizeof(req.ExchangeID));
    //    strcpy(req.InstrumentID, INSTRUMENT_ID);
    int iResult = TdApi->ReqQryInstrument(&req, ++iRequestTdID);
    // Sleep(1100);    // ?
    qDebug()<< QString::fromLocal8Bit("--->>> 请求查询合约: ") << ((iResult == 0) ? "sucess" : "failed");

    // ReqQryInstrumentCommissionRate();
}

// 请求查询合约手续费
void CtpTdApi::ReqQryInstrumentCommissionRate(QString instrument)
{
    QString ExchangeID = all_instrument_info[instrument].exchangeId;
    CThostFtdcQryInstrumentCommissionRateField req = { 0 };
    strncpy_s(req.BrokerID, usr_td_info.BrokerID, sizeof(req.BrokerID));
    strncpy_s(req.InvestorID, usr_td_info.UserID, sizeof(req.InvestorID));
//    strncpy_s(req.InstrumentID, instrument.toStdString().c_str(), sizeof(req.InstrumentID));
//    strncpy_s(req.ExchangeID, ExchangeID.toStdString().c_str(), sizeof(req.ExchangeID));
    int iResult = TdApi->ReqQryInstrumentCommissionRate(&req, ++iRequestTdID);
    qDebug()<< QString::fromLocal8Bit("--->>> 请求查询合约手续费: ") << usr_td_info.BrokerID<< usr_td_info.UserID<< instrument<< ExchangeID<< iResult<< ((iResult == 0) ? "sucess" : "failed");
}

// 请求查询资金账户
void CtpTdApi::ReqQryTradingAccount()
{
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, usr_td_info.BrokerID);
    strcpy(req.InvestorID, usr_td_info.UserID);
    int iResult = TdApi->ReqQryTradingAccount(&req, ++iRequestTdID);
    //    qDebug() << usr_td_info.UserID<< QString::fromLocal8Bit("--->>> 请求查询资金账户: ") << ((iResult == 0) ? "success" : "failed");
}

// 请求查询投资者持仓
void CtpTdApi::ReqQryInvestorPosition()
{
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, usr_td_info.BrokerID);
    strcpy(req.InvestorID, usr_td_info.UserID);
    //    strcpy(req.InstrumentID, INSTRUMENT_ID);
    int iResult = TdApi->ReqQryInvestorPosition(&req, ++iRequestTdID);
    //    qDebug() << usr_td_info.UserID<<QString::fromLocal8Bit("--->>> 请求查询投资者持仓: ") << ((iResult == 0) ? "success" : "failed");
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

void CtpTdApi::myDelayMSecond(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}



// Spi
// 连接第一步开始认证
void CtpTdApi::OnFrontConnected()
{
    is_td_connect = true;
    qDebug() << "--->>> " << "OnFrontConnected";
    ReqAuthenticate();
}

void CtpTdApi::OnFrontDisconnected(int nReason)
{
    is_td_connect = false;
    qDebug() << "--->>> " << "OnFrontDisconnected";
    qDebug() << "--->>> Reason = " << nReason;
}

void CtpTdApi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug()<< "CtpTdApi::OnRspAuthenticate"<< IsErrorRspInfo(pRspInfo)<< bIsLast;
    if (!IsErrorRspInfo(pRspInfo) && bIsLast)
    {
        qDebug() << QString::fromLocal8Bit("产品认证成功");
        ReqUserLogin();
    }
}

void CtpTdApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug()<< "--->>> " << "OnRspUserLogin"<< (!IsErrorRspInfo(pRspInfo) && bIsLast);
    if (!IsErrorRspInfo(pRspInfo) && bIsLast)
    {
        emit isTdConnect(usr_td_info.UserID);

        // 登录成功
        qDebug() << QString::fromLocal8Bit("TD交易登录成功");
        // 保存账户信息
        FRONT_ID = pRspUserLogin->FrontID;
        SESSION_ID = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        sprintf(ORDER_REF, "%d", iNextOrderRef);
        // 获取当前交易日
        qDebug() << QString::fromLocal8Bit("--->>> 获取当前交易日 = ") << TdApi->GetTradingDay();
        //  确认结算结果
        ReqSettlementInfoConfirm();
    }
}

void CtpTdApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pUserLogout)
    {
        // 退出标记
        is_td_logout = true;
        // 登出成功(所有qDebug的地方输出到日志监控模块)
        qDebug() << QString::fromLocal8Bit("TD交易成功退出");//TD  Succeed With  Logout
        emit isTdDisconnect(usr_td_info.UserID);
    }
}

void CtpTdApi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() <<"CtpTdApi::OnRspSettlementInfoConfirm"<< (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm);
    if (!IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm)
    {
        // 结算单确认以后才可交易
        is_td_tradable = true;

        // 结算单确认成功(所有qDebug的地方输出到日志监控模块)
        qDebug() << QString::fromLocal8Bit("TD SettlementConfirm确认成功");  //TD  Succeed With  SettlementConfirm

        //获取所有合约信息
        ReqQryInstrument();
    }
}

void CtpTdApi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() << "--->>> " << "OnRspOrderInsert";
}

void CtpTdApi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() << "--->>> " << "OnRspOrderAction" << endl;
}

// 订单回报
void CtpTdApi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    //获取最大报单引用
    usr_td_info.maxOrderRef = std::max(usr_td_info.maxOrderRef, (int)pOrder->OrderRef);
    std::shared_ptr<OrderInfo> order = std::move(std::shared_ptr<OrderInfo>(new OrderInfo));
    order->userId = usr_td_info.UserID;
    order->gatewayName = ("CTP");
    order->symbol = (pOrder->InstrumentID);
    order->exchange = (pOrder->ExchangeID);
    order->vtSymbol = (order->symbol);
    order->orderID = (pOrder->OrderRef);
    order->direction = (pOrder->Direction);
    order->offset = (pOrder->CombOffsetFlag[0]);
    order->status = (pOrder->OrderStatus);
    order->price = (pOrder->LimitPrice);
    order->totalVolume = (pOrder->VolumeTotalOriginal);
    order->tradeVolume = (pOrder->VolumeTraded);
    order->orderTime = (pOrder->InsertTime);
    order->cancelTime = (pOrder->CancelTime);
    order->frontID = (pOrder->FrontID);
    order->sessionID = (pOrder->SessionID);

    if(orderid_sender.contains(order->orderID.toInt()))
        order->tradeway = orderid_sender[order->orderID.toInt()];
    else
        order->tradeway = '-1';

    // CTP的报单号一致性维护需要基于frontID, sessionID, orderID三个字段
    // 但在本接口设计中，已经考虑了CTP的OrderRef的自增性，避免重复
    // 唯一可能出现OrderRef重复的情况是多处登录并在非常接近的时间内（几乎同时发单）
    // 考虑到VtTrader的应用场景，认为以上情况不会构成问题
    order->vtOrderID = (order->gatewayName + QString(".") + order->orderID);

    sendOrderInformation(order);

    // 委托单更新 -->ctptddata
    //    ctpTdOrderUpdate(*order);
}

// 成交回报
void CtpTdApi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    std::shared_ptr<TradeInfo> trade = std::move(std::shared_ptr<TradeInfo>(new TradeInfo)); // std::make_shared<QuoteInfo>(new QuoteInfo);
    trade->userId = usr_td_info.UserID;
    trade->gatewayName = ("CTP");
    trade->symbol = (pTrade->InstrumentID);
    trade->exchange = (pTrade->ExchangeID);
    trade->vtSymbol = (trade->symbol);
    trade->tradeID = (pTrade->TradeID);
    trade->vtTradeID = (trade->gatewayName + QString(".") + trade->tradeID);
    trade->orderID = (pTrade->OrderRef);        //委托编号
    trade->vtOrderID = (trade->gatewayName + QString(".") + trade->tradeID);
    trade->direction = (pTrade->Direction);
    trade->offset = (pTrade->OffsetFlag);
    trade->price = (pTrade->Price);
    trade->volume = (pTrade->Volume);
    trade->tradeTime = (pTrade->TradeTime);

    if(orderid_sender.contains(trade->orderID.toInt()))
        trade->tradeway = orderid_sender[trade->orderID.toInt()];
    else
        trade->tradeway = '-1';

    sendTradeInformation(trade);
}

// 持仓回报   more
void CtpTdApi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition)
    {
        //        qDebug()<< "CtpTdApi::OnRspQryInvestorPosition"
        //                << pInvestorPosition->PositionDate
        //                << "InstrumentID"<< pInvestorPosition->InstrumentID
        //                << "InvestorID"<< pInvestorPosition->InvestorID
        //                << "PosiDirection"<< pInvestorPosition->PosiDirection
        //                << "PositionDate"<< pInvestorPosition->PositionDate
        //                << "YdPosition"<< pInvestorPosition->YdPosition
        //                << "Position"<< pInvestorPosition->Position
        //                << "OpenVolume"<< pInvestorPosition->OpenVolume
        //                << "OpenAmount"<< pInvestorPosition->OpenAmount
        //                << "Commission"<< pInvestorPosition->Commission
        //                << "CloseProfit"<< pInvestorPosition->CloseProfit
        //                << "TradingDay"<< pInvestorPosition->TradingDay
        //                << "OpenCost"<< pInvestorPosition->OpenCost
        //                << "TodayPosition"<< pInvestorPosition->TodayPosition
        //                << "MarginRateByMoney"<< pInvestorPosition->MarginRateByMoney;


        std::shared_ptr<PositionInfo> position = std::move(std::shared_ptr<PositionInfo>(new PositionInfo));
        position->userId = pInvestorPosition->InvestorID;
        position->symbol = pInvestorPosition->InstrumentID;
        position->direction = pInvestorPosition->PosiDirection;
        position->position = pInvestorPosition->Position;
        position->ydPosition = pInvestorPosition->YdPosition;
        position->tdPosition = position->position - position->ydPosition;
        position->multiplier = all_instrument_info[position->symbol].multiplier;
        position->openCost = pInvestorPosition->OpenCost;
        position->averagePrice = position->position>0 ? (position->openCost / position->multiplier / position->position) : 0;
        position->exchangeId = all_instrument_info[position->symbol].exchangeId;
        position->gatewayName = QString("CTP");

        emit sendPositionInformation(position);
    }
}

// 账户查询回报
void CtpTdApi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //返回账户信息
    if (!IsErrorRspInfo(pRspInfo) && pTradingAccount)
    {
        std::shared_ptr<AccountInfo> account = std::move(std::shared_ptr<AccountInfo>(new AccountInfo));
        account->id = pTradingAccount->AccountID;		//帐户
        account->balance = pTradingAccount->Balance;	//总权益
        account->margin = pTradingAccount->CurrMargin; //占用保证金
        account->available = pTradingAccount->Available;	//可用资金
        account->commission = pTradingAccount->Commission;
        account->close_profit = pTradingAccount->CloseProfit;
        account->preBalance = pTradingAccount->PreBalance;
        account->position_profit = pTradingAccount->PositionProfit;
        account->gatewayName = ("CTP");

        emit sendAccountInformation(account);
        //        qDebug()<<account->id << " " <<account->balance << " "<<account->margin << " "<<account->available << " "<<account->commission ;

        //        qDebug()<< "emit sendAccount(account);";

        // update to de
        //        AccountInfo1 &de_account = de->accountInfo;
        //        de_account = *account;
    }

    // 查询账户后自动查询持仓
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        // 请求查询投资者持仓
        // 因为持仓查询具有延迟,无法正常显示,需要使用Sleep()
        // Sleep(2000); //windows.h
        ReqQryInvestorPosition();
    }
}

// 合约费率 回报
void CtpTdApi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
//    qDebug()<< "CtpTdApi::OnRspQryInstrumentCommissionRate"<< pInstrumentCommissionRate->OpenRatioByVolume
//            << pInstrumentCommissionRate->OpenRatioByMoney<< pInstrumentCommissionRate->OpenRatioByVolume
//            << pInstrumentCommissionRate->CloseRatioByMoney<< pInstrumentCommissionRate->CloseTodayRatioByVolume
//            << pInstrumentCommissionRate->CloseTodayRatioByMoney;
    //    if (!IsErrorRspInfo(pRspInfo) && pInstrumentCommissionRate)
    //    {
    //        // 获取数据
    //        QMap <QString, InstrumentInfo>&	allInstruments = de->allInstruments;

    //        // 此处只返回合约类型
    //        QString id = pInstrumentCommissionRate->InstrumentID;
    //        // 将合约中所有该类型的合约取出
    //        QStringList id_list;
    //        foreach(auto inst, allInstruments)
    //        {
    //            if (inst.id.contains(id))
    //            {
    //                id_list << inst.id;
    //            }
    //        }

    //        // 统一赋值费率
    //        foreach(auto inst, id_list)
    //        {
    //            if (allInstruments.find(inst) != allInstruments.end())
    //            {
    //                //如果存在
    //            }
    //            else
    //            {
    //                InstrumentInfo instrumentInfo;
    //                instrumentInfo.id = inst;
    //                allInstruments.insert(inst, instrumentInfo);
    //            }

    //            //写入内存对象中
    //            InstrumentInfo& info = allInstruments[inst];
    //            double &oc = pInstrumentCommissionRate->OpenRatioByVolume;
    //            double &oc_rate = pInstrumentCommissionRate->OpenRatioByMoney;
    //            double &cc = pInstrumentCommissionRate->OpenRatioByVolume;
    //            double &cc_rate = pInstrumentCommissionRate->CloseRatioByMoney;
    //            double &today_cc = pInstrumentCommissionRate->CloseTodayRatioByVolume;
    //            double &today_cc_rate = pInstrumentCommissionRate->CloseTodayRatioByMoney;
    //            info.openCommission = (oc > oc_rate ? oc : oc_rate);
    //            info.closeCommission = (cc > cc_rate ? cc : cc_rate);
    //            info.closeTodayCommission = (today_cc > today_cc_rate ? today_cc : today_cc_rate);
    //        }

    //        if (bIsLast)
    //        {
    //            // ee->putEvent(Event(EVENT_CONTRACT));
    //        }
    //    }
}

// 合约信息回报
void CtpTdApi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
//    qDebug()<< "CtpTdApi::OnRspQryInstrument"<< pInstrument->ProductID<< pInstrument->ProductClass<< pInstrument->InstrumentName<< pInstrument->ExchangeInstID;
    if (!IsErrorRspInfo(pRspInfo) && pInstrument && pInstrument->ProductClass == THOST_FTDC_PC_Futures)
    {
        InstrumentInfo instrument_info;
        instrument_info.symbol = pInstrument->InstrumentID;
        instrument_info.name = QString::fromLocal8Bit(pInstrument->InstrumentName);
        instrument_info.ProductID = pInstrument->ProductID;
        instrument_info.exchangeId = QString(pInstrument->ExchangeID);
        instrument_info.deadline = QDate::fromString(QString(pInstrument->ExpireDate), "yyyyMMdd");
        instrument_info.marginRate = pInstrument->LongMarginRatio;
        instrument_info.multiplier = pInstrument->VolumeMultiple;
        instrument_info.minimumUnit = pInstrument->PriceTick;
        instrument_info.tradable = pInstrument->IsTrading;
        emit sendInstrumentInformation(instrument_info);

        all_instrument_info[instrument_info.symbol] = instrument_info;
        all_instrument.append(instrument_info.symbol);

    }

    if (bIsLast)
    {
        InstrumentInfo instrument_info;
        instrument_info.is_end = true;
        emit sendInstrumentInformation(instrument_info);
        //  ReqQryTradingAccount();
        //  Sleep(1000);
        this->ReqQryInstrumentCommissionRate("ag1912");
    }
}
