#ifndef MAINENGINE_H
#define MAINENGINE_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "dataengine.h"
#include "ctpmdapi.h"
#include "ctptdapi.h"
#include "riskmanagement.h"

class MainEngine : public QObject
{
    Q_OBJECT
public:
    explicit MainEngine(QObject *parent = 0);
    ~MainEngine();

    //日志函数
    void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    // 返回 de指针
    DataEngine* getDataenginePointer() const;
    // me登录
    void meLogin(std::shared_ptr<LoginInformation>);
    // 行情登录
    void meCtpmdLogin(QString mdAddress);
    // 交易登录
    void meCtptdLogin(QString tdAddress, QString userid, QString password, QString brokerid,
                      QString AppID,QString AuthCode, QString UserProductInfo);
    // me退出
    void meLoginout();
    // me 订阅合约
    void meSubscribeInstrument(QString instrument);
    // 下单，返回订单编号
    QString meSendOrder(orderCommonRequest *order);
    // 临时的下单函数
    // QString meSendOrder(QString userID, QString exchangeID, QString Symbol, double price, int num, char direction, char offset);
    // 取消订单，0表示撤单成功
    int meCancelOrder(QString userid, cancelCommonRequest& cancel_field);
    // 一键全撤，取消所有用户的 所有挂单
    void meCancelAllorder(QString userid);
    // 根据返回交易所代码
    QString getExchangeid(QString symbol);
    // 返回合约信息，查询时调用
    QMap<QString, InstrumentInfo> &getInstrumentInfo();
    // 发送邮件
    void sendEmail(QString theme, QString content);
    // 返回主力合约 合集
    QSet<QString>& getMainInstrument();
    // 返回有期权的合约
    QStringList& getOptionList();
    // 是否是期权合约
    bool IsOption(QString symbol);
    // me订阅期权
    void meSubscribeOption(QString symbol);


    // 涨跌停价，最新价
    double getUpperLimitPrice(QString symbol);
    double getLowerLimitPrice(QString symbol);
    double getLastPrice(QString symbol);
    int getMultiplier(QString symbol);

signals:
    void repeatLoginError(QString);
    void tellTradeBoardAddUserid(QString);
    void tellTradeBoardDelUserid(QString);
    void sendLog(QString, QString, QString);
    void sendAccountInformation(std::shared_ptr<AccountInfo>);
    void sendOrderInformation(std::shared_ptr<OrderInfo>);
    void sendTradeInformation(std::shared_ptr<TradeInfo>);
    void sendPositionInformation(std::shared_ptr<PositionInfo>);
    void sendCancelAllOrder();
    void sendInformation(QString theme, QString content);

public slots:
    void receiveInstrumentInformation(InstrumentInfo);
    void receiveInstrumentCommissionRate(CommissionRate);
    void receiveOptionInformation(OptionInfo);
    void receiveAccountInformation(std::shared_ptr<AccountInfo>);
    void receiveOrderInformation(std::shared_ptr<OrderInfo>);
    void receiveTradeInformation(std::shared_ptr<TradeInfo>);
    void receivePositionInformation(std::shared_ptr<PositionInfo>);
    void receiveInformation(QString theme, QString content);

private:
    void addConnect();
    void ctpmdConnectToDataengine(CtpMdApi *md, DataEngine *de);
    void ctptdConnectToDataengine(CtpTdApi *td, DataEngine *de);
    void ctpmdConnectToMainengine(CtpMdApi *md);
    void ctptdConnectToMainengine(CtpTdApi *td);
    // 交易定时查询
    void ctptdQuery(CtpTdApi *td);
    // 查询获得主力合约
    void mdGetMainContract();
    // 是否获得到一个品种的所有合约持仓
    bool isGetAllInstrument(QList<QString>& list);

public:
    QMap<QString, OptionInfo> map_symbol_optioninfo;                //  合约：期权信息 ru2013P9300:[]
    QMap<QString, QList<QString>> map_symbol_optionlist;            //  ru2013:[ru2013P9300, ru2103P14750]
    QMap<QString, QList<double>> map_symbol_StrikePricelist;        //  ru2013:行权价增序
    QStringList m_option_list;

private:
    DataEngine *m_de {nullptr};
    RiskManagement *m_rm = nullptr;

    CtpMdApi* m_md{nullptr};
    QMap<QString, CtpTdApi*> m_userid_td;

    bool is_md_connect = false;
    bool is_get_all_instrument = false;                      // 是否获得了全市场合约
    bool is_subscribe_all_instrument = false;               // 是否订阅了全市场合约
    QMap<QString, bool> user_is_td_connect;                 // 账户 td的连接状态
    QMap<QString, int> instrument_openInterest;

    QMap<QString, InstrumentInfo> de_instrumentinfo;        //  合约：合约信息
    QMap<QString, QList<QString>> id_instrument_list;       //  ag:[ag1912, ag2001]


    QMap<QString, QString> id_main_instrument;              // ag: ag2006
    QSet<QString> main_instrument;                          // 主力合约集合
    QSet<QString> subscribe_instrument;                     // 用户主动订阅的合约集合
};

#endif // MAINENGINE_H
