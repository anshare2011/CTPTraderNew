#ifndef TAB_ORDER_TRADE_H
#define TAB_ORDER_TRADE_H

#include <QWidget>
#include <QTableWidget>
#include <QStringList>
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include "publicstruct.h"
#include "mywidgets/mytabwidget.h"
#include "mainengine.h"

extern MainEngine* m_me;

class Tab_Order_Trade : public MyTabWidget
{
    Q_OBJECT
public:
    explicit Tab_Order_Trade(QWidget *parent = 0);
    ~Tab_Order_Trade();

signals:

public slots:
    void receiveOrderInformation(std::shared_ptr<OrderInfo>);
    void receiveTradeInformation(std::shared_ptr<TradeInfo>);
    void receiveStopWinLossOrder(stopWinLossOrder);
    // 撤单槽函数
    void slotCancelOrder(const QPoint&);
    int cancelOneOrder();

    void slotCancelStopOrder(const QPoint&);
    void cancelStopOrder();

    // 撤销所有未完成的委托
    void cancelAllOrder();

private:
    void initOrderTable();
    void initTradeTable();
    void initCondition();
    void initStopWinLoss();

    // 订单表右键撤单
    void orderTableSetCancelOrder();
    // 止盈止损右键取消
    void stopwinlossTableSetCancel();

private:
    QTableWidget *table_currentorder{nullptr};
    QStringList name_currentorder;
    QTableWidget *table_historytrade{nullptr};
    QStringList name_historytrade;
    QTableWidget *table_condition{nullptr};
    QStringList name_condition;
    QTableWidget *table_stopwinloss{nullptr};
    QStringList name_stopwinloss;

    QMenu *menu_order;
    QAction *action_cancel_order;
    QMenu *menu_stopwinloss;
    QAction *action_cancel_stopwinloss;
};

#endif // TAB_ORDER_TRADE_H
