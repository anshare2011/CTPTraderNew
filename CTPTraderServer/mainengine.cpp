#include "mainengine.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>


MainEngine::MainEngine(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<InstrumentInfo>("InstrumentInfo");
    qRegisterMetaType<std::shared_ptr<QuoteInfo>>("std::shared_ptr<QuoteInfo>");
}

void MainEngine::meLogin(std::shared_ptr<LoginInformation> login)
{
    mdAddress = login->mdAddress;
    tdAddress = login->tdAddress;
    userName = login->userName;
    password = login->password;
    brokerID = login->brokerID;
    AppID = login->AppID;
    AuthCode = login->authorizationCode;
    productionInfo = login->productionInfo;

    this->meCtpmdLogin(mdAddress);
    this->meCtptdLogin(tdAddress, userName, password, brokerID, AppID, AuthCode, productionInfo);
}

void MainEngine::meCtpmdLogin(QString mdAddress)
{
    if(!is_md_connect)
    {
        md = new CtpMdApi();
        this->ctpmdConnectToMainengine(md);
        md->ctpMdConnectToExchange(mdAddress);
    }
    else
    {
        qDebug()<< "md is connected, so"<< mdAddress << "is not connect to md";
    }
}

void MainEngine::meCtptdLogin(QString tdAddress, QString userid, QString password, QString brokerid,
                              QString AppID, QString AuthCode, QString UserProductInfo)
{
    // is_login = true;
    if(!is_td_connect)
    {
        td = new CtpTdApi();
        this->ctptdConnectToMainengine(td);
        td->ctpTdConnectToExchange(tdAddress, userid, password, brokerid,
                                   AppID, AuthCode, UserProductInfo);
    }
}

void MainEngine::startCollectData()
{
    // 第一次没登录mdAddress tdAddress为空，定时登录会出错
    if(mdAddress.size() > 1 && tdAddress.size() >1)
    {
        this->meCtpmdLogin(mdAddress);
        this->meCtptdLogin(tdAddress, userName, password, brokerID, AppID, AuthCode, productionInfo);
    }
}

void MainEngine::stopCollectData()
{
    // is_login = false;
    //停止行情
    if(md != nullptr){
        md->ctpMdRelease();
        delete md;
        md = nullptr;
    }

    //停止交易
    if(td != nullptr){
        td->ctpTdRelease();
        delete td;
        td = nullptr;
    }
    is_md_connect = false;
    is_td_connect = false;

    // 断开释放资源
    //    for(int i = 0;i< md_list.size();++i){
    //        md_list.at(i)->ctpMdRelease();
    //        delete md_list.at(i);
    //    }
    md_list.clear();

    // 断开释放资源
    //    for(int i = 0;i< de_list.size();++i){
    //        delete de_list.at(i);
    //    }
    de_list.clear();
}

void MainEngine::ctpmdConnectToMainengine(CtpMdApi *md)
{
    connect(md, &CtpMdApi::sendMdConnect, [=](){
        this->is_md_connect = true;
    });

    connect(md, &CtpMdApi::sendMdDisconnect, [=](){
        this->is_md_connect = false;
    });
}

void MainEngine::ctptdConnectToMainengine(CtpTdApi *td)
{
    connect(td, &CtpTdApi::isTdConnect, [=](QString user_id){
        is_td_connect = true;
    });

    connect(td, &CtpTdApi::isTdDisconnect, [=](QString user_id){
        is_td_connect = false;
    });

    connect(td, SIGNAL(sendInstrumentInformation(InstrumentInfo)),
            this, SLOT(receiveInstrumentInformation(InstrumentInfo)));
}

void MainEngine::receiveInstrumentInformation(InstrumentInfo instrument)
{
    emit sendInstrumentInformation(instrument);

    // qDebug()<< "MainEngine::receiveInstrumentInformation"<< instrument.symbol<< instrument.exchangeId;
    if(!instrument.is_end)
    {
        if(exchangeName_symbolList.contains(instrument.exchangeId))
        {
            exchangeName_symbolList[instrument.exchangeId].append(instrument.symbol);
        }
        else{
            QList<QString> list;
            list.append(instrument.symbol);
            exchangeName_symbolList.insert(instrument.exchangeId, list);
        }

        all_symbol.append(instrument.symbol);
    }
    else{
        qDebug()<< exchangeName_symbolList;
        qDebug()<< "-------------------------------------------------------------------------------";
        QList<QString> &list = exchangeName_symbolList["SHFE"];
        qDebug()<<all_symbol ;
        qDebug()<< "-------------------------------------------------------------------------------";
        qDebug()<<list ;
        // return;

        //        for(auto it = exchangeName_symbolList.begin(); it != exchangeName_symbolList.end(); ++it)
        //        {
        //            QString exchangeName = it.key();
        //            QList<QString> &list = it.value();

        //            CtpMdApi* md1 = new CtpMdApi();
        //            DataEngine* de = new DataEngine(exchangeName);
        //            QThread * thread = new QThread();
        //            connect(md1, SIGNAL(sendQuoteInformation(std::shared_ptr<QuoteInfo>)),
        //                    de, SLOT(receiveQuoteInformation(std::shared_ptr<QuoteInfo>)));

        //            //开启线程
        //            de->moveToThread(thread);
        //            thread->start();

        //            md_list.append(md1);
        //            de_list.append(de);

        //            md1->ctpMdConnectToExchange(mdAddress);
        //            std::this_thread::sleep_for (std::chrono::seconds(5));
        //            for(int i = 0;i< list.size(); ++i){
        //                qDebug()<< "md ctpMdSubscribeInstrument"<< list.at(i);
        //                md1->ctpMdSubscribeInstrument(list.at(i));
        //            }
        //            break;
        //        }
        //        return;


        CtpMdApi* md = new CtpMdApi();
        DataEngine* de = new DataEngine("SHFE");
        QThread * thread = new QThread();
        connect(md, SIGNAL(sendQuoteInformation(std::shared_ptr<QuoteInfo>)),
                de, SLOT(receiveQuoteInformation(std::shared_ptr<QuoteInfo>)));

        //开启线程
        de->moveToThread(thread);
        thread->start();

        md_list.append(md);
        de_list.append(de);

        md->ctpMdConnectToExchange(mdAddress);
        std::this_thread::sleep_for (std::chrono::seconds(5));
        for(auto it = exchangeName_symbolList.begin(); it != exchangeName_symbolList.end(); ++it)
        {
            QList<QString> &list = it.value();
            for(int i = 0;i< list.size(); ++i){
                md->ctpMdSubscribeInstrument(list.at(i));
            }
        }


        //        for(int i = 0;i< list.size(); ++i){
        //            qDebug()<< "md ctpMdSubscribeInstrument"<< list.at(i);
        //            md->ctpMdSubscribeInstrument(list.at(i));
        //        }
    }

    //    exchangeName_symbolList.clear();

}

// login 为 false表示断开连接
void MainEngine::controlIsLogin(bool login)
{
    if(login && is_td_connect == false)
    {
        this->startCollectData();
    }
    else if(!login)
    {
        this->stopCollectData();
    }
}
