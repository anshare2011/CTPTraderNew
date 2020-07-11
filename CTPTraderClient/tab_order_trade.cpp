#include "tab_order_trade.h"

Tab_Order_Trade::Tab_Order_Trade(QWidget *parent)
{
    this->setMaximumHeight(320);
    this->setTabsClosable(false);
    this->setTabPosition(QTabWidget::South);

    QString str = tr("QTabBar::tab{width: %1px;height:24px;font:15px;color:black}").arg(100);
    str.append("QTabWidget::pane{border-width: 0px;border-color:white;border-style:outset;background-color: rgb(69, 69, 69);border-radius: 3px;}");
    str.append("QTabBar::tab:selected{margin-left: 1;margin-right: 0;color: black;background-color:rbg(50, 100, 100);} ");
    str.append("QTabBar::tab:!selected{margin-left: 1;margin-right: 0;color: white;background-color:rbg(69, 69, 69);} ");
    str.append("QTabBar::tab:hover:!selected{color: white;margin-left: 1;margin-right: 0;background-color:rbg(100,200,100);} ");
    this->setStyleSheet(str);
    this->setDocumentMode(true);

    this->initOrderTable();
    this->initTradeTable();
    this->initCondition();
    this->initStopWinLoss();

    this->orderTableSetCancelOrder();
    this->stopwinlossTableSetCancel();
}

Tab_Order_Trade::~Tab_Order_Trade()
{
    delete table_currentorder;
    delete table_historytrade;
    delete table_stopwinloss;
}

void Tab_Order_Trade::receiveOrderInformation(std::shared_ptr<OrderInfo> order)
{
    for(int i=0 ;i<table_currentorder->rowCount(); i++)
    {
        //根据合约 委托编号 会话编号来 判断委托回报是否在表中
        if (table_currentorder->item(i, 0)->text() ==  order->userId &&
                table_currentorder->item(i, 1)->text() == order->orderID &&
                table_currentorder->item(i, 12)->text() == QString::number(order->sessionID))
        {
            table_currentorder->item(i, 2)->setText(order->symbol);
            table_currentorder->item(i, 3)->setText(order->exchange);
            if('0' == order->direction)  //0买入
            {
                table_currentorder->item(i, 4)->setText(QString::fromLocal8Bit("买 多"));
                if(table_currentorder->item(i,4)->backgroundColor() != Qt::darkRed)
                {
                    table_currentorder->item(i,4)->setBackgroundColor(Qt::darkRed);
                }
            }
            else if('1' == order->direction)  //1卖出
            {
                table_currentorder->item(i, 4)->setText(QString::fromLocal8Bit("卖 空"));
                if(table_currentorder->item(i,4)->backgroundColor() != Qt::darkGreen)
                {
                    table_currentorder->item(i,4)->setBackgroundColor(Qt::darkGreen);
                }
            }
            else
            {
                table_currentorder->item(i, 4)->setText(QString::fromLocal8Bit("uncertain direction"));
            }

            if('0' == order->offset)
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("开 仓"));
            }
            else if('1' == order->offset)
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("平 仓"));
            }
            else if('2' == order->offset)
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("强 平"));
            }
            else if('3' == order->offset)
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("平 今"));
            }
            else if('4' == order->offset)
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("平 昨"));
            }
            else
            {
                table_currentorder->item(i, 5)->setText(QString::fromLocal8Bit("uncertain offset"));
            }
            table_currentorder->item(i, 6)->setText(QString::number(order->price));
            table_currentorder->item(i, 7)->setText(QString::number(order->totalVolume));
            table_currentorder->item(i, 8)->setText(QString::number(order->tradeVolume));

            if('0' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("全部成交"));
            }
            else if('1' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("部分成交还在队列"));
                table_currentorder->item(i,9)->setBackgroundColor(Qt::blue);
            }
            else if('2' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("部分成交不在队列"));
            }
            else if('3' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("未成交还在队列"));
                table_currentorder->item(i,9)->setBackgroundColor(Qt::blue);
            }
            else if('4' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("未成交不在队列"));
            }
            else if('5' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("已撤单"));
            }
            else if('a' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("未知"));
            }
            else if('b' == order->status)
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("尚未触发"));
            }
            else
            {
                table_currentorder->item(i, 9)->setText(QString::fromLocal8Bit("已触发"));
            }
            table_currentorder->item(i, 10)->setText(order->orderTime);
            table_currentorder->item(i, 11)->setText(QString::number(order->frontID));
            table_currentorder->item(i, 12)->setText(QString::number(order->sessionID));

            return;
        }
    }
    // 不在表中就插入
    int row =table_currentorder->rowCount();
    table_currentorder->insertRow(row);
    table_currentorder->setItem(row, 0, new QTableWidgetItem(order->userId));
    table_currentorder->setItem(row, 1, new QTableWidgetItem(order->orderID));
    table_currentorder->setItem(row, 2, new QTableWidgetItem(order->symbol));
    table_currentorder->setItem(row, 3, new QTableWidgetItem(order->exchange));
    if('0' == order->direction)  //0买入
    {
        table_currentorder->setItem(row, 4, new QTableWidgetItem(QString::fromLocal8Bit("买 多")));
        table_currentorder->item(row,4)->setBackgroundColor(Qt::darkRed);
    }
    else if('1' == order->direction)  //1卖出
    {
        table_currentorder->setItem(row, 4, new QTableWidgetItem(QString::fromLocal8Bit("卖 空")));
        table_currentorder->item(row, 4)->setBackgroundColor(Qt::darkGreen);
    }
    else
    {
        table_currentorder->setItem(row, 4, new QTableWidgetItem(QString::fromLocal8Bit("uncertain direction")));
    }

    if('0' == order->offset)
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("开 仓")));
    }
    else if('1' == order->offset)
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("平 仓")));
    }
    else if('2' == order->offset)
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("强 平")));
    }
    else if('3' == order->offset)
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("平 今")));
    }
    else if('4' == order->offset)
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("平 昨")));
    }
    else
    {
        table_currentorder->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("uncertain offset")));
    }
    table_currentorder->setItem(row, 6, new QTableWidgetItem(QString::number(order->price)));
    table_currentorder->setItem(row, 7, new QTableWidgetItem(QString::number(order->totalVolume)));
    table_currentorder->setItem(row, 8, new QTableWidgetItem(QString::number(order->tradeVolume)));
    if('0' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("全部成交")));
    }
    else if('1' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("部分成交还在队列")));
        table_currentorder->item(row, 9)->setBackgroundColor(Qt::blue);
    }
    else if('2' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("部分成交不在队列")));
    }
    else if('3' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("未成交还在队列")));
        table_currentorder->item(row, 9)->setBackgroundColor(Qt::blue);
    }
    else if('4' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("未成交不在队列")));
    }
    else if('5' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("已撤单")));
    }
    else if('a' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("未知")));
    }
    else if('b' == order->status)
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("尚未触发")));
    }
    else
    {
        table_currentorder->setItem(row, 9, new QTableWidgetItem(QString::fromLocal8Bit("已触发")));
    }
    table_currentorder->setItem(row, 10, new QTableWidgetItem(order->orderTime));
    table_currentorder->setItem(row, 11, new QTableWidgetItem(QString::number(order->frontID)));
    table_currentorder->setItem(row, 12, new QTableWidgetItem(QString::number(order->sessionID)));
    table_currentorder->setItem(row, 13, new QTableWidgetItem(order->gatewayName));

    if('0' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("手动下单")));
    else if('1' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("快捷下单")));
    else if('2' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("策略下单")));
    else if('3' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("手动平仓")));
    else if('4' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("RPC")));
    else if('5' == order->tradeway)
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("止盈止损平")));
    else
        table_currentorder->setItem(row, 14, new QTableWidgetItem(QString::fromLocal8Bit("不确定")));
}

void Tab_Order_Trade::receiveTradeInformation(std::shared_ptr<TradeInfo> trade)
{
    int row =table_historytrade->rowCount();
    table_historytrade->insertRow(row);
    table_historytrade->setItem(row, 0, new QTableWidgetItem(trade->userId));
    table_historytrade->setItem(row, 1, new QTableWidgetItem(trade->symbol));
    table_historytrade->setItem(row, 3, new QTableWidgetItem(trade->exchange));
    table_historytrade->setItem(row, 4, new QTableWidgetItem(trade->tradeID));
    if('0' == trade->direction)  //0买入
    {
        table_historytrade->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("买 多")));
        table_historytrade->item(row,5)->setBackgroundColor(Qt::darkRed);
    }
    else if('1' == trade->direction)  //1卖出
    {
        table_historytrade->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("卖 空")));
        table_historytrade->item(row, 5)->setBackgroundColor(Qt::darkGreen);
    }
    else
    {
        table_historytrade->setItem(row, 5, new QTableWidgetItem(QString::fromLocal8Bit("uncertain direction")));
    }

    if('0' == trade->offset)
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("开 仓")));
    }
    else if('1' == trade->offset)
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("平 仓")));
    }
    else if('2' == trade->offset)
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("强 平")));
    }
    else if('3' == trade->offset)
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("平 今")));
    }
    else if('4' == trade->offset)
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("平 昨")));
    }
    else
    {
        table_historytrade->setItem(row, 6, new QTableWidgetItem(QString::fromLocal8Bit("uncertain offset")));
    }
    table_historytrade->setItem(row, 7, new QTableWidgetItem(QString::number(trade->price)));
    table_historytrade->setItem(row, 8, new QTableWidgetItem(QString::number(trade->volume)));
    table_historytrade->setItem(row, 9, new QTableWidgetItem(trade->tradeTime));
    table_historytrade->setItem(row, 10, new QTableWidgetItem(trade->gatewayName));
    table_historytrade->setItem(row, 11, new QTableWidgetItem(trade->orderID));

    if('0' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("手动下单")));
    else if('1' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("快捷下单")));
    else if('2' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("策略下单")));
    else if('3' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("手动平仓")));
    else if('4' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("RPC")));
    else if('5' == trade->tradeway)
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("止盈止损平")));
    else
        table_historytrade->setItem(row, 12, new QTableWidgetItem(QString::fromLocal8Bit("不确定")));

}

void Tab_Order_Trade::receiveStopWinLossOrder(stopWinLossOrder)
{

}

void Tab_Order_Trade::slotCancelOrder(const QPoint &pt)
{
    menu_order->exec(this->table_currentorder->mapToGlobal(pt));
}

int Tab_Order_Trade::cancelOneOrder()
{
    int ret = -1;
    int i = table_currentorder->currentIndex().row();         //委托表所在的行数
    if(-1 ==i)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("撤单失败"),QString::fromLocal8Bit("委托表格为空,请确认!"),QMessageBox::Ok);
    }
    else
    {
        int total = table_currentorder->item(i, 7)->text().toInt();
        int trade = table_currentorder->item(i, 8)->text().toInt();
        QString status = table_currentorder->item(i, 9)->text();
        if(total == trade || status ==QString::fromLocal8Bit("已撤单"))
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("撤单失败"),QString::fromLocal8Bit("未成交数量为0,或已撤单,请确认!"),QMessageBox::Ok);
        }
        else
        {
            //从委托表所在行获得撤单的信息
            cancelCommonRequest cancel_field;
            QString userid = table_currentorder->item(i, 0)->text();
            cancel_field.order_ref = table_currentorder->item(i, 1)->text();
            cancel_field.instrument = table_currentorder->item(i, 2)->text();
            cancel_field.exchange = table_currentorder->item(i, 3)->text();
            cancel_field.front_id = table_currentorder->item(i, 11)->text().toInt();        // 前置编号
            cancel_field.session_id = table_currentorder->item(i, 12)->text().toInt();      // 会话编号
            ret = m_me->meCancelOrder(userid, cancel_field);
        }
    }
    return ret;
}

void Tab_Order_Trade::slotCancelStopOrder(const QPoint &pt)
{
    menu_stopwinloss->exec(this->table_stopwinloss->mapToGlobal(pt));
}

void Tab_Order_Trade::cancelStopOrder()
{

}

void Tab_Order_Trade::cancelAllOrder()
{
    for(int i = 0; i< table_currentorder->rowCount(); ++i)
    {
        int total = table_currentorder->item(i, 7)->text().toInt();
        int trade = table_currentorder->item(i, 8)->text().toInt();
        QString status = table_currentorder->item(i, 9)->text();
        if(total == trade || status ==QString::fromLocal8Bit("已撤单"))
        {

        }
        else
        {
            //从委托表所在行获得撤单的信息
            cancelCommonRequest cancel_field;
            QString userid = table_currentorder->item(i, 0)->text();
            cancel_field.order_ref = table_currentorder->item(i, 1)->text();
            cancel_field.instrument = table_currentorder->item(i, 2)->text();
            cancel_field.exchange = table_currentorder->item(i, 3)->text();
            cancel_field.front_id = table_currentorder->item(i, 11)->text().toInt();        // 前置编号
            cancel_field.session_id = table_currentorder->item(i, 12)->text().toInt();      // 会话编号
            m_me->meCancelOrder(userid, cancel_field);
        }
    }
}

void Tab_Order_Trade::initOrderTable()
{
    table_currentorder = new QTableWidget(this);
    table_currentorder->setAutoFillBackground(true);
    table_currentorder->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_currentorder->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_currentorder->setParent(this);
    this->addTab(table_currentorder, QString::fromLocal8Bit("当前委托"));

    name_currentorder.empty();
    name_currentorder.append(QString::fromLocal8Bit("账户"));
    name_currentorder.append(QString::fromLocal8Bit("委托编号"));
    name_currentorder.append(QString::fromLocal8Bit("合约代码"));
    name_currentorder.append(QString::fromLocal8Bit("交易所"));
    name_currentorder.append(QString::fromLocal8Bit("方向"));
    name_currentorder.append(QString::fromLocal8Bit("开平"));
    name_currentorder.append(QString::fromLocal8Bit("价格"));
    name_currentorder.append(QString::fromLocal8Bit("委托数量"));
    name_currentorder.append(QString::fromLocal8Bit("成交数量"));
    name_currentorder.append(QString::fromLocal8Bit("状态"));
    name_currentorder.append(QString::fromLocal8Bit("委托时间"));
    name_currentorder.append(QString::fromLocal8Bit("前置编号"));
    name_currentorder.append(QString::fromLocal8Bit("会话编号"));
    name_currentorder.append(QString::fromLocal8Bit("接口"));
    name_currentorder.append(QString::fromLocal8Bit("发单类型"));
    table_currentorder->setColumnCount(name_currentorder.size());
    table_currentorder->setHorizontalHeaderLabels(name_currentorder);
    table_currentorder->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    table_currentorder->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_currentorder->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_currentorder->verticalHeader()->setHidden(true);                               // 隐藏行号
}

void Tab_Order_Trade::initTradeTable()
{
    table_historytrade = new QTableWidget();
    table_historytrade->setAutoFillBackground(true);
    table_historytrade->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_historytrade->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_historytrade->setParent(this);
    this->addTab(table_historytrade, QString::fromLocal8Bit("历史成交"));

    name_historytrade.empty();
    name_historytrade.append(QString::fromLocal8Bit("账户"));
    name_historytrade.append(QString::fromLocal8Bit("合约代码"));
    name_historytrade.append(QString::fromLocal8Bit("名称"));
    name_historytrade.append(QString::fromLocal8Bit("交易所"));
    name_historytrade.append(QString::fromLocal8Bit("交易ID"));
    name_historytrade.append(QString::fromLocal8Bit("方向"));
    name_historytrade.append(QString::fromLocal8Bit("开平"));
    name_historytrade.append(QString::fromLocal8Bit("价格"));
    name_historytrade.append(QString::fromLocal8Bit("数量"));
    name_historytrade.append(QString::fromLocal8Bit("交易时间"));
    name_historytrade.append(QString::fromLocal8Bit("接口"));
    name_historytrade.append(QString::fromLocal8Bit("委托编号"));
    name_historytrade.append(QString::fromLocal8Bit("发单类型"));
    table_historytrade->setColumnCount(name_historytrade.size());
    table_historytrade->setHorizontalHeaderLabels(name_historytrade);
    table_historytrade->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);    // Stretch ResizeToContents);  //将表头显示在表内
    table_historytrade->setSelectionBehavior(QAbstractItemView::SelectRows);                        // 选中一行
    table_historytrade->setEditTriggers(QAbstractItemView::NoEditTriggers);                         // 表格不可编辑
    table_historytrade->verticalHeader()->setHidden(true);
}

void Tab_Order_Trade::initCondition()
{
    table_condition = new QTableWidget();
    table_condition->setAutoFillBackground(true);
    table_condition->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_condition->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_condition->setParent(this);
    this->addTab(table_condition, QString::fromLocal8Bit("条件单"));
}

void Tab_Order_Trade::initStopWinLoss()
{
    table_stopwinloss = new QTableWidget();
    table_stopwinloss->setAutoFillBackground(true);
    table_stopwinloss->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_stopwinloss->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_stopwinloss->setParent(this);
    this->addTab(table_stopwinloss, QString::fromLocal8Bit("止盈止损单"));

    name_stopwinloss.empty();
    name_stopwinloss.append(QString::fromLocal8Bit("编号"));
    name_stopwinloss.append(QString::fromLocal8Bit("时间"));
    name_stopwinloss.append(QString::fromLocal8Bit("用户"));
    name_stopwinloss.append(QString::fromLocal8Bit("合约"));
    name_stopwinloss.append(QString::fromLocal8Bit("原始方向"));
    name_stopwinloss.append(QString::fromLocal8Bit("状态"));    // 运行中，失效
    name_stopwinloss.append(QString::fromLocal8Bit("类型"));
    name_stopwinloss.append(QString::fromLocal8Bit("触发价"));
    name_stopwinloss.append(QString::fromLocal8Bit("数量"));
    name_stopwinloss.append(QString::fromLocal8Bit("平仓下单方式"));
    name_stopwinloss.append(QString::fromLocal8Bit("预计盈亏"));
    name_stopwinloss.append(QString::fromLocal8Bit("有效区间"));


    table_stopwinloss->setColumnCount(name_stopwinloss.size());
    table_stopwinloss->setHorizontalHeaderLabels(name_stopwinloss);
    table_stopwinloss->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    table_stopwinloss->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_stopwinloss->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_stopwinloss->verticalHeader()->setHidden(true);
}

void Tab_Order_Trade::orderTableSetCancelOrder()
{
    menu_order = new QMenu(this->table_currentorder);
    action_cancel_order = new QAction(QString::fromLocal8Bit("取消订单"), this);
    menu_order->addAction(action_cancel_order);

    this->table_currentorder->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table_currentorder, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotCancelOrder(const QPoint&)));
    connect(action_cancel_order, SIGNAL(triggered(bool)), this, SLOT(cancelOneOrder()));
}

void Tab_Order_Trade::stopwinlossTableSetCancel()
{

}
