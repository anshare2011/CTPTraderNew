#ifndef CTPTDAPI_H
#define CTPTDAPI_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QDateTime>
#include <QDate>
#include <memory>
#include <string>
#include <QEventLoop>
#include <QTimer>
//#include <windows.h>
#include <algorithm>      // max
#include "ctpApi/ThostFtdcTraderApi.h"
#include "ctpApi/ThostFtdcUserApiDataType.h"
#include "ctpApi/ThostFtdcUserApiStruct.h"
#include "publicstruct.h"

class CtpTdApi : public QObject, public CThostFtdcTraderSpi
{
    Q_OBJECT
public:
    explicit CtpTdApi(QObject *parent = 0);

signals:
    void sendTradeInformation(std::shared_ptr<TradeInfo>);
    void sendOrderInformation(std::shared_ptr<OrderInfo>);
    void sendPositionInformation(std::shared_ptr<PositionInfo>);
    void sendAccountInformation(std::shared_ptr<AccountInfo>);
    void sendInstrumentInformation(InstrumentInfo);
    void sendUserid(QString);
    void isTdConnect(QString);
    void isTdDisconnect(QString);

public:
    // 获取连接和登录状态
    bool getIsTdConnect();
    bool getIsTdLogout();

    // td连接交易所
    void ctpTdConnectToExchange(QString tdAddress, QString userid, QString password, QString brokerid, QString AppID, QString AuthCode, QString UserProductInfo);
    // 释放td资源
    void ctpTdRelease();
    // 查询刷新页面
    void ctpTdQuery();

    // 委托单买卖撤单查
    QString ctpTdSendLimitOrder(QString exchangeid, QString instrument, double price, int volume, char direction, char offset, char tradeway);
    QString ctpTdSendMarketOrder(QString exchangeid, QString instrumentid, double volume, char direction, char offset, char tradeway);
    // 委托单公共字段填写并下单
    QString ctpTdSendOrder(CThostFtdcInputOrderField &orderField);
    int ctpTdCancelOrder(QString instrumentID, QString exchangeID, QString orderID, int frontID, int sessionID);


public slots:

private:
    // 认证
    void ReqAuthenticate();
    // 用户登录登录
    void ReqUserLogin();
    // 用户登录登出
    void ReqUserLogout();

    // 交易API基本信息查询
    void ReqSettlementInfoConfirm();
    void ReqQryInstrument();
    void ReqQryInstrumentCommissionRate(QString instrument);
    void ReqQryTradingAccount();
    void ReqQryInvestorPosition();

    // 判断是否错误消息
    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    // 是否我的报单回报
    bool IsMyOrder(CThostFtdcOrderField *pOrder);
    // 是否正在交易的报单
    bool IsTradingOrder(CThostFtdcOrderField *pOrder);
    // 委托单状态更新
    //    void ctpTdOrderUpdate(OrderInfo& order);
    // 委托单公共字段填写并下单
    //    QString ctpTdSendOrder(CThostFtdcInputOrderField &orderField);

    void myDelayMSecond(unsigned int msec);

private:
    //交易API
    CThostFtdcTraderApi* TdApi{nullptr};
    // 用户交易信息
    struct MyAccount
    {
        char FRONT_ADDR[100];
        TThostFtdcBrokerIDType	BrokerID;
        TThostFtdcUserIDType	UserID;
        TThostFtdcPasswordType	PassWord;
        // 用户端产品信息
        TThostFtdcProductInfoType  UserProductInfo;
        // 产品名称
        TThostFtdcAppIDType       AppID;
        // 授权编码
        TThostFtdcAuthInfoType  AuthCode;

        //请求编号
        int requestID{ 0 };     // del
        //本地的最大报单引用
        int maxOrderRef{ 0 };    // use

        // 会话标记
        int front_id{ 0 };
        int session_id{ 0 };
    }usr_td_info;


    //是否连接/可交易/登出
    bool is_td_connect{ false };
    bool is_td_tradable{ false };
    bool is_td_logout{ false };
    // 查询事件计数
    int query_count{ 0 };
    int query_trgger{ 3 };
    int query_function_index{ 0 };
    //请求编号
    int iRequestTdID{0};
    // 合约代码
    //    char INSTRUMENT_ID[100]= { 0 };
    // 前置地址
    char frontaddress[512];
    // 会话参数
    TThostFtdcFrontIDType	FRONT_ID;	//前置编号
    TThostFtdcSessionIDType	SESSION_ID;	//会话编号
    TThostFtdcOrderRefType	ORDER_REF;	//报单引用
    
    QMap<int, QChar> orderid_sender;    // 本地维护一个 委托编号-发送者，从而在委托表成交表中显示是谁发送的。
    QMap<QString, InstrumentInfo> all_instrument_info;
    QList<QString> all_instrument;
    

private:
    /****************************Spi回调函数****************************************/
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnFrontConnected() override;
    ///当客户端与交易后台通信连接断开时，该方法被调用。
    void OnFrontDisconnected(int nReason) override;
    ///客户端认证响应
    void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)  override;
    ///登录请求响应
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///登出请求响应
    void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///投资者结算结果确认响应
    void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///报单录入请求响应
    void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///撤单操作请求响应
    void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///报单通知
    void OnRtnOrder(CThostFtdcOrderField *pOrder) override;
    ///成交通知
    void OnRtnTrade(CThostFtdcTradeField *pTrade) override;
    ///请求查询投资者持仓响应
    void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///请求查询资金账户响应
    void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///请求查询合约手续费率响应
    void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    ///请求查询合约响应
    void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
};

#endif // CTPTDAPI_H
