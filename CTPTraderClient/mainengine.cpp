#include "mainengine.h"

MainEngine::MainEngine(QObject *parent) : QObject(parent)
{
    m_de = new DataEngine();
    m_rm = new RiskManagement();
}

MainEngine::~MainEngine()
{
    delete m_de;
}

void MainEngine::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString level = "";
    switch(type)
    {
    case QtDebugMsg:
        level = QString("Debug");
        break;

    case QtWarningMsg:
        level = QString("Warning");
        break;

    case QtCriticalMsg:
        level = QString("Critical");
        break;

    case QtFatalMsg:
        level = QString("Fatal");

    default:
        level = QString("");
    }

    QString context_info = QString("F:(%1) L:(%2)").arg(QString(context.file)).arg(context.line); // F文件信息L行数
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("DateTime:%1, Level:%2, Message:%3, context:%4")
            .arg(current_date_time, level, msg, context_info);

    QString log_path = "log/";
    QDir temp;
    if (!temp.exists(log_path))
    {
        bool n = temp.mkpath(log_path);   //Creates the directory path dirPath.
    }

    // 文件名以日期进行区分
    QString filename = log_path + QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt");
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();
    // 解锁
    mutex.unlock();
    emit sendLog(current_date_time, level ,msg);
}

DataEngine *MainEngine::getDataenginePointer() const
{
    return m_de;
}

void MainEngine::meLogin(std::shared_ptr<LoginInformation> login)
{
    QString mdAddress = login->mdAddress;
    QString tdAddress = login->tdAddress;
    QString brokerid = login->brokerID;
    QString userid = login->userName;
    QString password = login->password;
    QString AppID = login->AppID;
    QString AuthCode = login->authorizationCode;
    QString UserProductInfo = login->productionInfo;

    this->meCtpmdLogin(mdAddress);
    this->meCtptdLogin(tdAddress, userid, password, brokerid, AppID, AuthCode, UserProductInfo);
}

void MainEngine::meCtpmdLogin(QString mdAddress)
{
    if(!is_md_connect)
    {
        m_md = new CtpMdApi();
        m_md->ctpMdConnectToExchange(mdAddress);
        this->ctpmdConnectToDataengine(m_md, m_de);
        this->ctpmdConnectToMainengine(m_md);
    }
    else
    {
        qDebug()<< QString::fromLocal8Bit("期货行情地址")<< mdAddress << QString::fromLocal8Bit("已经连接，不需要重复连接");
    }
}

void MainEngine::meCtptdLogin(QString tdAddress, QString userid, QString password, QString brokerid, QString AppID, QString AuthCode, QString UserProductInfo)
{
    // 不包含，第一次登录
    if(!user_is_td_connect.contains(userid))
    {
        CtpTdApi* td = new CtpTdApi();
        this->ctptdConnectToDataengine(td, m_de);
        this->ctptdConnectToMainengine(td);
        this->ctptdQuery(td);
        td->ctpTdConnectToExchange(tdAddress, userid, password, brokerid, AppID, AuthCode, UserProductInfo);
        m_userid_td.insert(userid, td);
    }
    // 登陆过，但未连接成功
    else if(user_is_td_connect[userid] == false)
    {
        CtpTdApi* td = new CtpTdApi();
        this->ctptdConnectToDataengine(td, m_de);
        this->ctptdConnectToMainengine(td);
        this->ctptdQuery(td);
        td->ctpTdConnectToExchange(tdAddress, userid, password, brokerid, AppID, AuthCode, UserProductInfo);
        // 赋值前 删除之前的指针 ？
        m_userid_td[userid] = td;
    }
    else
    {
        qDebug() << QString::fromLocal8Bit("用户:") << userid << QString::fromLocal8Bit("已经连接，不能重复连接");
    }
}

void MainEngine::meLoginout()
{

}

void MainEngine::meSubscribeInstrument(QString instrument)
{
    if(nullptr != m_md){
        m_md->ctpMdSubscribeInstrument(instrument);
        subscribe_instrument.insert(instrument);
        m_de->addSubscribeInstrument(instrument);
    }
}

// 给所有登录的用户进行下单
QString MainEngine::meSendOrder(orderCommonRequest* order)
{
    QString OrderRef = "";
    if(order->userid == QString("ALL"))
    {
        QMap<QString, CtpTdApi *>::const_iterator it;
        for(it= m_userid_td.constBegin(); it!= m_userid_td.constEnd(); ++it)
        {
            OrderRef = it.value()->ctpTdSendLimitOrder(order->exchangeid, order->instrument, order->price,
                                                       order->quantity, order->direction, order->offset, '0');
        }
    }
    else
    {
        if(m_userid_td.contains(order->userid))
        {
            CtpTdApi *td = m_userid_td[order->userid];
            OrderRef = td->ctpTdSendLimitOrder(order->exchangeid, order->instrument, order->price,
                                               order->quantity, order->direction, order->offset, '0');
        }
        else
        {
            qDebug()<< QString::fromLocal8Bit("下单账户不存在");
        }
    }
    return OrderRef;
}

//QString MainEngine::meSendOrder(QString userID, QString exchangeID, QString Symbol, double price, int num, char direction, char offset)
//{
//    QString OrderRef = "";
//    if(m_userid_td.contains(userID) && user_is_td_connect[userID])
//    {
//        CtpTdApi *td = m_userid_td[userID];
//        OrderRef = td->ctpTdSendLimitOrder(exchangeID, Symbol, price,
//                                           num, direction, offset, '0');
//    }
//    return OrderRef;
//}

int MainEngine::meCancelOrder(QString userid, cancelCommonRequest &cancel_field)
{
    if(m_userid_td.contains(userid)){
        CtpTdApi* td = m_userid_td[userid];
        return td->ctpTdCancelOrder(cancel_field.instrument,
                                    cancel_field.exchange,
                                    cancel_field.order_ref,
                                    cancel_field.front_id,
                                    cancel_field.session_id);

    }
    else{
        return -1;
    }
}

void MainEngine::meCancelAllorder(QString userid)
{
    emit sendCancelAllOrder();
}

QString MainEngine::getExchangeid(QString symbol)
{
    if(de_instrumentinfo.contains(symbol))
        return de_instrumentinfo[symbol].exchangeId;
    else
        return "";
}

QMap<QString, InstrumentInfo> &MainEngine::getInstrumentInfo()
{
    return de_instrumentinfo;
}

void MainEngine::sendEmail(QString theme, QString content)
{

}

QSet<QString> &MainEngine::getMainInstrument()
{
    return main_instrument;
}

QStringList &MainEngine::getOptionList()
{
    return m_option_list;
}

bool MainEngine::IsOption(QString symbol)
{
    return map_symbol_optionlist.contains(symbol);
}

void MainEngine::meSubscribeOption(QString symbol)
{
    QList<QString> optionlist = map_symbol_optionlist[symbol];
    foreach(QString option, optionlist)
    {
        m_md->ctpMdSubscribeInstrument(option);
    }
}

double MainEngine::getUpperLimitPrice(QString symbol)
{
    double price = -1;
    OneSymbolKlineData* data =  m_de->returnOneSymbolKlineData(symbol);
    if(nullptr != data)
        price = data->upperLimitPrice;
    return price;
}

double MainEngine::getLowerLimitPrice(QString symbol)
{
    double price = -1;
    OneSymbolKlineData* data =  m_de->returnOneSymbolKlineData(symbol);
    if(nullptr != data)
        price = data->lowerLimitPrice;
    return price;
}

double MainEngine::getLastPrice(QString symbol)
{
    double price = -1;
    OneSymbolKlineData* data =  m_de->returnOneSymbolKlineData(symbol);
    if(nullptr != data)
        price = data->m_lastPrice;
    return price;
}

int MainEngine::getMultiplier(QString symbol)
{
    int multiplier = -1;
    if(de_instrumentinfo.contains(symbol))
        multiplier = de_instrumentinfo[symbol].multiplier;
    return multiplier;
}

void MainEngine::receiveInstrumentInformation(InstrumentInfo info)
{
    m_de->updataInstrumentInfo(info);

    // 多账户登录，这里只需要进来一次
    if(is_get_all_instrument) return;

    if(!info.is_end){
        QString symbol = info.symbol;
        QString ProductID = info.ProductID;
        if(!de_instrumentinfo.contains(symbol)){
            de_instrumentinfo[symbol] = info;
        }

        if(id_instrument_list.contains(ProductID))
        {
            id_instrument_list[ProductID].append(symbol);
        }
        else{
            QList<QString> temp;
            temp.append(symbol);
            id_instrument_list.insert(ProductID, temp);
        }
    }
    else{
        // 获得到了全部合约， 查询主力合约
        is_get_all_instrument = true;

        // m_option_list;
        for(auto it = map_symbol_optionlist.begin(); it != map_symbol_optionlist.end(); ++it)
        {
            m_option_list<< it.key();
        }

        this->mdGetMainContract();
    }
}

void MainEngine::receiveInstrumentCommissionRate(CommissionRate info)
{
    QString InstrumentID = info.InstrumentID;
    if(id_instrument_list.contains(InstrumentID))
    {
        QList<QString>& symbol_list = id_instrument_list[InstrumentID];
        foreach(QString symbol, symbol_list)
        {
            if(de_instrumentinfo.contains(symbol))
            {
                de_instrumentinfo[symbol].openCommission = info.openRatio;
                de_instrumentinfo[symbol].closeCommission = info.closeRatio;
                de_instrumentinfo[symbol].closeTodayCommission = info.closeTodayRatio;
            }
        }
    }
}

void MainEngine::receiveOptionInformation(OptionInfo info)
{
    //    QMap<QString, OptionInfo> map_symbol_optioninfo;                //  合约：期权信息 ru2013P9300:
    //    QMap<QString, QList<QString>> map_symbol_optionlist;            //  ru2013:[ru2013P9300, ru2103P14750]
    QString symbol =  info.symbol;                                      // ru2103P14750
    QString UnderlyingSymbol = info.UnderlyingSymbol;                   // ru2103P14750 -> ru2013
    double struckPrice = info.StrikePrice;

    if(!map_symbol_optioninfo.contains(symbol))
    {
        map_symbol_optioninfo[symbol] = info;

        if(map_symbol_optionlist.contains(UnderlyingSymbol))
        {
            map_symbol_optionlist[UnderlyingSymbol].append(symbol);

            // 14750 会推两次
            QList<double> &priceList = map_symbol_StrikePricelist[UnderlyingSymbol];
            if(struckPrice < priceList.at(0))
                priceList.insert(0, struckPrice);
            else if(struckPrice > priceList.at(priceList.size()-1))
                priceList.append(struckPrice);
            else
            {
                for(int i = 0; i<priceList.size() -1; ++i)
                {
                    if(struckPrice > priceList.at(i) && struckPrice < priceList.at(i+1))
                    {
                        priceList.insert(i+1, struckPrice);
                        break;
                    }
                }
            }

        }
        else
        {
            QList<QString> symbolList;
            QList<double> priceList;
            symbolList.append(symbol);
            priceList.append(struckPrice);
            map_symbol_optionlist.insert(UnderlyingSymbol, symbolList);
            map_symbol_StrikePricelist.insert(UnderlyingSymbol, priceList);
        }
    }

}

void MainEngine::receiveAccountInformation(std::shared_ptr<AccountInfo> account)
{
    emit sendAccountInformation(account);
}

void MainEngine::receiveOrderInformation(std::shared_ptr<OrderInfo> order)
{
    emit sendOrderInformation(order);
}

void MainEngine::receiveTradeInformation(std::shared_ptr<TradeInfo> trade)
{
    emit sendTradeInformation(trade);
}

void MainEngine::receivePositionInformation(std::shared_ptr<PositionInfo> position)
{
    QString symbol = position->symbol;
    // 主动订阅合约
    if(!subscribe_instrument.contains(symbol))
    {
        subscribe_instrument.insert(symbol);
        this->meSubscribeInstrument(symbol);
    }

    position->multiplier = this->getMultiplier(symbol);
    position->lastPrice = this->getLastPrice(symbol);
    if(-1 != position->multiplier)
    {
        position->averagePrice = position->position>0? position->openCost/ position->multiplier /position->position : 0 ;
    }

    if(-1 != position->multiplier && -1 != position->lastPrice)
    {
        position->is_cal = true;
        if('2' == position->direction){
            position->winLoss = (position->lastPrice - position->averagePrice) * position->multiplier * position->position;
        }
        else if('3' == position->direction){
            position->winLoss = (position->averagePrice - position->lastPrice) * position->multiplier * position->position;
        }
    }
    emit sendPositionInformation(position);
}

void MainEngine::receiveInformation(QString theme, QString content)
{
    emit sendInformation(theme, content);
}

void MainEngine::addConnect()
{

}

// 将行情转发给 Dataengine 处理
void MainEngine::ctpmdConnectToDataengine(CtpMdApi *md, DataEngine *de)
{
    connect(md, SIGNAL(sendQuoteInformation(std::shared_ptr<QuoteInfo>)), de, SLOT(receiveQuoteInformation(std::shared_ptr<QuoteInfo>)));
}

void MainEngine::ctptdConnectToDataengine(CtpTdApi *td, DataEngine *de)
{

}

// 行情对象告诉 MainEngine 是否连接
void MainEngine::ctpmdConnectToMainengine(CtpMdApi *md)
{
    connect(md, &CtpMdApi::sendMdConnect, [=](){
        this->is_md_connect = true;
    });

    connect(md, &CtpMdApi::sendMdDisconnect, [=](){
        this->is_md_connect = false;
    });

    connect(md, &CtpMdApi::sendOpenInterest, [=](QString symbol, double openInterest, double amount){
        if(de_instrumentinfo.contains(symbol)){
            de_instrumentinfo[symbol].openInterest = openInterest;
            de_instrumentinfo[symbol].tempVolume = amount;
            instrument_openInterest[symbol] = openInterest;
        }
    });
}

void MainEngine::ctptdConnectToMainengine(CtpTdApi *td)
{
    //    connect(td, &CtpTdApi::sendInstrumentInformation, [=](InstrumentInfo info){
    //        // 多账户登录，这里只需要进来一次
    //        if(is_get_all_instrument) return;

    //        if(!info.is_end){
    //            QString symbol = info.symbol;
    //            QString ProductID = info.ProductID;
    //            if(!de_instrumentinfo.contains(symbol)){
    //                de_instrumentinfo[symbol] = info;
    //            }

    //            if(id_instrument_list.contains(ProductID))
    //            {
    //                id_instrument_list[ProductID].append(symbol);
    //            }
    //            else{
    //                QList<QString> temp;
    //                temp.append(symbol);
    //                id_instrument_list.insert(ProductID, temp);
    //            }
    //        }
    //        else{
    //            // 获得到了全部合约， 查询主力合约
    //            is_get_all_instrument = true;
    //            this->mdGetMainContract();
    //        }
    //    });

    connect(td, SIGNAL(sendInstrumentInformation(InstrumentInfo)),
            this, SLOT(receiveInstrumentInformation(InstrumentInfo)));

    connect(td, SIGNAL(sendInstrumentCommissionRate(CommissionRate)),
            this, SLOT(receiveInstrumentCommissionRate(CommissionRate)));

    connect(td, SIGNAL(sendOptionInformation(OptionInfo)),
            this, SLOT(receiveOptionInformation(OptionInfo)));

    connect(td, &CtpTdApi::isTdConnect, [=](QString user_id){
        user_is_td_connect[user_id] = true;
        emit tellTradeBoardAddUserid(user_id);
    });

    connect(td, &CtpTdApi::isTdDisconnect, [=](QString user_id){
        user_is_td_connect[user_id] = false;
        emit tellTradeBoardDelUserid(user_id);
    });

    connect(td, SIGNAL(sendTdInformation(QString, QString)), this, SLOT(receiveInformation(QString, QString)));

    // 账户信息
    connect(td, SIGNAL(sendAccountInformation(std::shared_ptr<AccountInfo>)),
            this, SLOT(receiveAccountInformation(std::shared_ptr<AccountInfo>)));

    // 订单信息
    connect(td, SIGNAL(sendOrderInformation(std::shared_ptr<OrderInfo>)),
            this, SLOT(receiveOrderInformation(std::shared_ptr<OrderInfo>)));

    // 成交信息
    connect(td, SIGNAL(sendTradeInformation(std::shared_ptr<TradeInfo>)),
            this, SLOT(receiveTradeInformation(std::shared_ptr<TradeInfo>)));

    // 持仓信息
    connect(td, SIGNAL(sendPositionInformation(std::shared_ptr<PositionInfo>)),
            this, SLOT(receivePositionInformation(std::shared_ptr<PositionInfo>)));

}

void MainEngine::ctptdQuery(CtpTdApi *td)
{
    QTimer* updateTimer = new QTimer(this);
    updateTimer->setSingleShot(false);
    connect(updateTimer,&QTimer::timeout,[=](){
        td->ctpTdQuery();
    });
    updateTimer->start(2000);
}

void MainEngine::mdGetMainContract()
{
    // qDebug()<< "MainEngine::mdGetMainContrac"<< m_md<< is_md_connect;
    QTimer* updateTimer = new QTimer(this);
    updateTimer->setSingleShot(false);
    connect(updateTimer,&QTimer::timeout,[=](){
        if(!is_subscribe_all_instrument){
            if(nullptr != m_md && is_md_connect){
                is_subscribe_all_instrument = true;
                qDebug()<< QString::fromLocal8Bit("MainEngine::mdGetMainContract 订阅合约的总数") << id_instrument_list.size();
                m_md->ctpMdGetMainContract(id_instrument_list);
            }

        }
        // 订阅全市场以后，  instrument_openInterest   和  id_instrument_list  判断主力，取消非主力订阅
        else{
            for(auto it = id_instrument_list.begin(); it != id_instrument_list.end(); ++it)
            {
                if(id_main_instrument.contains(it.key())) continue;

                QList<QString>& instrument_list = it.value();

                // 所有的都拿到了
                if(isGetAllInstrument(instrument_list)){
                    QString instrument = instrument_list.at(0);
                    int open_interest = instrument_openInterest[instrument];
                    for(int i = 1; i< instrument_list.size(); ++i){
                        if(instrument_openInterest[instrument_list.at(i)] > open_interest){
                            instrument = instrument_list.at(i);
                            open_interest = instrument_openInterest[instrument_list.at(i)];
                        }
                    }
                    id_main_instrument[it.key()] = instrument;
                    main_instrument.insert(instrument);
                    // 取消非主力的订阅
                    for(int i = 1; i< instrument_list.size(); ++i){
                        if(instrument_list.at(i) != instrument)
                            m_md->ctpMdUnsubscribeInstrument( instrument_list.at(i) );
                    }
                }

            }
        }
    });
    updateTimer->start(2000);
}

bool MainEngine::isGetAllInstrument(QList<QString> &list)
{
    bool ret = true;
    for(int i = 0; i< list.size(); ++i)
    {
        if(!instrument_openInterest.contains(list[i]))
            ret = false;
    }
    return ret;
}
