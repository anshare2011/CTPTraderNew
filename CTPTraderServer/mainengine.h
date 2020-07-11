#ifndef MAINENGINE_H
#define MAINENGINE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <memory>
#include <QThread>
#include "ctpmdapi.h"
#include "ctptdapi.h"
#include "dataengine.h"
#include "publicstruct.h"

class MainEngine : public QObject
{
    Q_OBJECT
public:
    explicit MainEngine(QObject *parent = 0);

    // me登录
    void meLogin(std::shared_ptr<LoginInformation>);
    // 行情登录
    void meCtpmdLogin(QString mdAddress);
    // 交易登录
    void meCtptdLogin(QString tdAddress, QString userid, QString password, QString brokerid, QString AppID,QString AuthCode, QString UserProductInfo);


    void startCollectData();
    void stopCollectData();

    void ctpmdConnectToMainengine(CtpMdApi *md);
    void ctptdConnectToMainengine(CtpTdApi *td);

signals:
    void sendInstrumentInformation(InstrumentInfo);

public slots:
    void receiveInstrumentInformation(InstrumentInfo);

    void controlIsLogin(bool login);

private:
    // 登录信息
    QString mdAddress = "";             // "tcp://180.168.146.187:10131";  //"tcp://180.168.146.187:10110";    //  tcp://180.168.146.187:10131
    QString tdAddress = "";             // "tcp://180.168.146.187:10130";  //"tcp://180.168.146.187:10100";    //  tcp://180.168.146.187:10130
    QString brokerID = "";              // "9999";
    QString userName = "";              // "121995";
    QString password = "";              // "Hxw19930922";
    QString AppID = "";        // "simnow_client_test";    // 产品名称
    QString AuthCode = "";     // ="0000000000000000";    // 授权编码
    QString productionInfo = "";        //"";    // 产品信息

    QMap<QString, QList<QString> > exchangeName_symbolList;   // 交易所：对应的合约列表

    bool is_login {false};
    CtpTdApi* td;
    CtpMdApi* md;
    QList<CtpMdApi*> md_list;
    QList<DataEngine*> de_list;
    QList<QString> all_symbol;

    bool is_md_connect {false};
    bool is_td_connect {false};

};

#endif // MAINENGINE_H
