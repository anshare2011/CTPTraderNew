#ifndef CTPMDAPI_H
#define CTPMDAPI_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QDir>
#include <memory>
#include "ctpApi/ThostFtdcMdApi.h"
#include "publicstruct.h"

class CtpMdApi : public QObject, public CThostFtdcMdSpi
{
    Q_OBJECT
public:
    explicit CtpMdApi(QObject *parent = 0);
    //判断是否连接
    bool getIsMdConnect();
    bool getIsMdLogout();
    //md连接交易所
    void ctpMdConnectToExchange(QString mdaddress);
    //释放md资源
    void ctpMdRelease();
    //md登录
    void ctpMdLogin();
    //md退出出
    void ctpMdLogout();
    //md订阅合约
    void ctpMdSubscribeInstrument(QString instrumendName, bool inside = false);
    // 取消订阅
    void ctpMdUnsubscribeInstrument(QString instrumendName);
    // 订阅全市场，获得主力合约，然后取消非主力合约的订阅
    void ctpMdGetMainContract(QMap<QString, QList<QString>>& id_instrument_list);

signals:
    void sendQuoteInformation(std::shared_ptr<QuoteInfo>);
    // md是否连接 信号
    void sendMdConnect();
    void sendMdDisconnect();
    // 发送持仓量 和 成交量
    void sendOpenInterest(QString symbol, double openInterest ,double amount);

public slots:

private:
    CThostFtdcMdApi *MdApi{nullptr};
    int request_id{0};
    bool is_md_login{ false };
    bool is_md_logout{ false };

    QSet <QString> user_subscribed_instruments_set;    // 用户订阅合约合集

private:
    /****************************Spi回调函数****************************************/
    //当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用
    void OnFrontConnected() override;
    //当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    void OnFrontDisconnected(int nReason) override;
    //登录请求响应
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    //登出请求响应
    void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    //行情回报响应
    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
    //错误应答
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

#endif // CTPMDAPI_H
