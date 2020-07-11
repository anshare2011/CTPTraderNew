#ifndef TRADEBOARD_H
#define TRADEBOARD_H

#include <QWidget>
#include <QPaintEvent>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <memory>
#include "publicstruct.h"
#include "mainengine.h"
#include "ui/setstopwinloss.h"
#include "ui/setconditionorder.h"

extern MainEngine* m_me;

const int begin_1 = 10;        // 第一列框的起始位置
const int begin_2 = 110;
const int begin_3 = 210;
const int begin_4 = 310;

const int height_1  = 30;      // 买一卖一框的高度
const int height_2  = 20;      // 下面框的高度
const int height_3  = 30;      // 下面框占的高度
const int height_4  = 24;      // 下面框的真实高度

const int length_1 = 80;       // 第一个框的长度


class Tradeboard : public QWidget
{
    Q_OBJECT
public:
    explicit Tradeboard(QWidget *parent = 0);

    ~Tradeboard();   

    // 返回面板上的交易信息
    BoardInfo getBoardInfo();
    // 接收快捷交易
    void receiveQuickType(QuickType type);

public slots:
    // 接收行情
    void receiveQuoteInformation(std::shared_ptr<QuoteInfo>);
    // 主界面合约发生变化
    void changeInstrument(QString);
    // 添加用户名
    void addAccount(QString);
    // 删除用户名
    void delAccount(QString);

protected:
    // 画边框
    void paintEvent(QPaintEvent *event) override;

private:
    // 面板初始化
    void tradeboardInit();
    // 事件过滤
    bool eventFilter(QObject *target, QEvent *event) override;
    // 点击下单时从面板获得订单信息
    bool getOrderInfo(orderCommonRequest *order_info);
    //
    void addConnect();

private:

    SetStopWinLoss *m_winLoss = nullptr;

    SetConditionOrder *m_condition = nullptr;

    struct tradeboardnumber{
        QLabel *instrument_info;
        QLabel *instrument_show;

        QLabel *AskPrice1;
        QLabel *AskPrice1_show;
        QLabel *AskPrice1_vol_show;

        QLabel *BidPrice1;
        QLabel *BidPrice1_show;
        QLabel *BidPrice1_vol_show;

        QLabel *lastPrice;
        QLabel *lastPrice_show;
        QLabel *preSettlementPrice;
        QLabel *preSettlementPrice_show;

        QLabel *zhangDie;
        QLabel *zhangDie_show;
        QLabel *zuoshouPrice;
        QLabel *zuoshouPrice_show;

        QLabel *fuDu;
        QLabel *fuDu_show;
        QLabel *openPrice;
        QLabel *openPrice_show;

        QLabel *zhenFu;
        QLabel *zhenFu_show;
        QLabel *highPrice;
        QLabel *highPrice_show;

        QLabel *dealAmount;
        QLabel *dealAmount_show;
        QLabel *lowPrice;
        QLabel *lowPrice_show;

        QLabel *holdingAmount;
        QLabel *holdingAmount_show;
        QLabel *upperPrice;
        QLabel *upperPrice_show;

        QLabel *timenow;
        QLabel *timenow_show;
        QLabel *lowerPrice;
        QLabel *lowerPrice_show;

        QLabel *account;
        QComboBox *account_com;
        QLabel *price;
        QDoubleSpinBox *instrumentPrice_spin;

        QLabel *openclose;
        QComboBox *openclose_com;
        QLabel *pricetype;
        QComboBox *pricetype_com;

        QLabel *direction;
        QComboBox *direction_com;
        QLabel *amount;
        QSpinBox *amount_spin;

        QPushButton * xiadan;
        QPushButton * cancel;

        QLabel *set_winloss;
        QLabel *set_condition;
    }tradeboard;

};

#endif // TRADEBOARD_H
