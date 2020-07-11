#include "tab_account_position.h"

Tab_Account_Position::Tab_Account_Position(QWidget *parent)
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

    this->initAccountTable();
    this->initPositionTable();
    this->initLogTable();

    this->positionTableOffset();
}

Tab_Account_Position::~Tab_Account_Position()
{
    delete table_account;
    delete table_poistion;
    delete table_log;
}

// 相同合约相同方向的订单 一次查询可能会返回多条
void Tab_Account_Position::receiveChiCangInformation(std::shared_ptr<PositionInfo> position)
{
    if(position->is_end)
        return;

    //判断是否在持仓表中
    for(int i=0 ;i<table_poistion->rowCount(); i++)
    {
        if(table_poistion->item(i, 0)->text() == position->userId &&
                table_poistion->item(i, 1)->text() == position->symbol &&
                table_poistion->item(i, 2)->text() == QString(position->direction))   //根据合约代码与买卖方向确定
        {
            table_poistion->item(i, 4)->setText(QString::number(position->position));
            table_poistion->item(i, 5)->setText(QString::number(position->position - position->ydPosition));
            table_poistion->item(i, 6)->setText(QString::number(position->ydPosition));
            if(position->lastPrice <0)
                table_poistion->item(i, 7)->setText(QString::fromLocal8Bit("未知"));
            else
                table_poistion->item(i, 7)->setText(QString::number(position->lastPrice));
            table_poistion->item(i, 8)->setText(QString::number(position->openCost));
            table_poistion->item(i, 9)->setText(QString::number(position->averagePrice));
            table_poistion->item(i, 10)->setText(QString::number(position->winLoss));
            return;
        }
    }
    int row = table_poistion->rowCount();
    table_poistion->insertRow(row);
    table_poistion->setItem(row, 0, new QTableWidgetItem(position->userId));
    table_poistion->setItem(row, 1, new QTableWidgetItem(position->symbol));
    table_poistion->setItem(row, 2, new QTableWidgetItem(QString(position->direction)));

    if('2' == position->direction)  //0买入
    {
        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("买 多")));
        table_poistion->item(row,3)->setBackgroundColor(Qt::darkRed);

    }
    else if('3' == position->direction)  //1卖出
    {
        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("卖 空")));
        table_poistion->item(row, 3)->setBackgroundColor(Qt::darkGreen);
    }
    else
    {
        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("净")));
    }
    table_poistion->setItem(row, 4, new QTableWidgetItem(QString::number(position->position)));
    table_poistion->setItem(row, 5, new QTableWidgetItem(QString::number(position->position - position->ydPosition)));
    table_poistion->setItem(row, 6, new QTableWidgetItem(QString::number(position->ydPosition)));
    if(position->lastPrice <0)
        table_poistion->setItem(row, 7, new QTableWidgetItem(QString::fromLocal8Bit("未知")));
    else
        table_poistion->setItem(row, 7, new QTableWidgetItem(QString::number(position->lastPrice)));
    table_poistion->setItem(row, 8, new QTableWidgetItem(QString::number(position->openCost)));
    table_poistion->setItem(row, 9, new QTableWidgetItem(QString::number(position->averagePrice)));
    table_poistion->setItem(row, 10, new QTableWidgetItem(QString::number(position->winLoss)));
    table_poistion->setItem(row, 11, new QTableWidgetItem(position->exchangeId));
    table_poistion->setItem(row, 12, new QTableWidgetItem(position->gatewayName));







//    if(position->is_end)
//    {
//        this->position_clear = true;
//        return;
//    }
//    if(this->position_clear)
//    {
//        this->position_clear = false;
//        while(table_poistion->rowCount() > 0)
//        {
//            table_poistion->removeRow(0);
//        }
//    }

//    int row = table_poistion->rowCount();
//    table_poistion->insertRow(row);
//    table_poistion->setItem(row, 0, new QTableWidgetItem(position->userId));
//    table_poistion->setItem(row, 1, new QTableWidgetItem(position->symbol));
//    table_poistion->setItem(row, 2, new QTableWidgetItem(QString(position->direction)));

//    if('2' == position->direction)  //0买入
//    {
//        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("买 多")));
//        table_poistion->item(row,3)->setBackgroundColor(Qt::darkRed);

//    }
//    else if('3' == position->direction)  //1卖出
//    {
//        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("卖 空")));
//        table_poistion->item(row, 3)->setBackgroundColor(Qt::darkGreen);
//    }
//    else
//    {
//        table_poistion->setItem(row, 3, new QTableWidgetItem(QString::fromLocal8Bit("净")));
//    }
//    table_poistion->setItem(row, 4, new QTableWidgetItem(QString::number(position->position)));
//    table_poistion->setItem(row, 5, new QTableWidgetItem(QString::number(position->position - position->ydPosition)));
//    table_poistion->setItem(row, 6, new QTableWidgetItem(QString::number(position->ydPosition)));
//    if(position->lastPrice <0)
//        table_poistion->setItem(row, 7, new QTableWidgetItem(QString::fromLocal8Bit("未知")));
//    else
//        table_poistion->setItem(row, 7, new QTableWidgetItem(QString::number(position->lastPrice)));
//    table_poistion->setItem(row, 8, new QTableWidgetItem(QString::number(position->openCost)));
//    table_poistion->setItem(row, 9, new QTableWidgetItem(QString::number(position->averagePrice)));
//    table_poistion->setItem(row, 10, new QTableWidgetItem(QString::number(position->winLoss)));
//    table_poistion->setItem(row, 11, new QTableWidgetItem(position->exchangeId));
//    table_poistion->setItem(row, 12, new QTableWidgetItem(position->gatewayName));
    // table_poistion->setItem(row, 13, new QTableWidgetItem(position->SettlementID));
}

void Tab_Account_Position::receiveAccountInformation(std::shared_ptr<AccountInfo> account)
{
    for(int i=0 ;i<table_account->rowCount(); ++i)
    {
        if(table_account->item(i, 0)->text() == account->id)
        {
            // only 2-8 need to change
            table_account->item(i, 2)->setText(QString::number(account->balance,10,2));
            table_account->item(i, 3)->setText(QString::number(account->available,10,2));
            table_account->item(i, 4)->setText(QString::number(account->commission,10,2));
            table_account->item(i, 5)->setText(QString::number(account->margin,10,2));
            table_account->item(i, 6)->setText(QString::number(account->close_profit,10,2));
            table_account->item(i, 7)->setText(QString::number(account->position_profit,10,2));
            table_account->item(i, 8)->setText(QString::number(account->margin/account->balance *100,10,2));
            return;
        }
    }

    int row = table_account->rowCount();
    table_account->insertRow(row);
    table_account->setItem(row, 0, new QTableWidgetItem(account->id));
    table_account->setItem(row, 1, new QTableWidgetItem(QString::number(account->preBalance,10,2)));
    table_account->setItem(row, 2, new QTableWidgetItem(QString::number(account->balance,10,2)));
    table_account->setItem(row, 3, new QTableWidgetItem(QString::number(account->available,10,2)));
    table_account->setItem(row, 4, new QTableWidgetItem(QString::number(account->commission,10,2)));
    table_account->setItem(row, 5, new QTableWidgetItem(QString::number(account->margin,10,2)));
    table_account->setItem(row, 6, new QTableWidgetItem(QString::number(account->close_profit,10,2)));
    table_account->setItem(row, 7, new QTableWidgetItem(QString::number(account->position_profit,10,2)));
    table_account->setItem(row, 8, new QTableWidgetItem(QString::number(account->margin/account->balance *100,10,2)));
    table_account->item(row,8)->setBackgroundColor(Qt::darkRed);
    table_account->setItem(row, 9, new QTableWidgetItem(account->gatewayName));
}

void Tab_Account_Position::receiveLog(QString current_date_time, QString level, QString msg)
{
    int row =table_log->rowCount();
    table_log->insertRow(row);
    table_log->setItem(row, 0, new QTableWidgetItem(current_date_time));
    table_log->setItem(row, 1, new QTableWidgetItem(level));
    table_log->setItem(row, 2, new QTableWidgetItem(msg));
}

void Tab_Account_Position::slotOffsetPosition(const QPoint &pt)
{
    menu_position->exec(this->table_poistion->mapToGlobal(pt));
}

// 平掉一笔持仓
void Tab_Account_Position::offsetOnePosition()
{
    int i = table_poistion->currentIndex().row();         //委托表所在的行数
    if(-1 ==i)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("平仓失败"),QString::fromLocal8Bit("委托表格为空,请确认!"),QMessageBox::Ok);
    }
    else{
        QString account_name = table_poistion->item(i, 0)->text();
        QString symbol = table_poistion->item(i, 1)->text();
        QString direction = table_poistion->item(i, 2)->text();                     // "2" 多头， "3"空头
        int position = table_poistion->item(i, 4)->text().toInt();
        int td_position = table_poistion->item(i, 5)->text().toInt();
        int yd_position = table_poistion->item(i, 6)->text().toInt();
        QString exchange_name = table_poistion->item(i, 11)->text();
        double upperPrice = m_me->getUpperLimitPrice(symbol);
        double lowerPrice = m_me->getLowerLimitPrice(symbol);
        if(0 == position)
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("平仓失败"),QString::fromLocal8Bit("可平数量为0,请确认!"),QMessageBox::Ok);
            return;
        }
        if(upperPrice <0 || lowerPrice<0)
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("平仓失败"),QString::fromLocal8Bit("获取涨跌停价失败!"),QMessageBox::Ok);
            return;
        }

        orderCommonRequest order;
        order.instrument = symbol;
        order.quantity = position;
        order.pricetype = '0';      // '0'表示市价， '1'表示限价， simnow只支持限价
        if("2" == direction)        // "2" 多头， "3"空头
        {
            order.direction = THOST_FTDC_D_Sell;
            order.price = lowerPrice;
        }
        else{
            order.direction = THOST_FTDC_D_Buy;
            order.price = upperPrice;
        }
        // order.localType = 3;
        order.exchangeid = exchange_name;
        order.userid = account_name;

        if("SHFE" == exchange_name)
        {
            if(td_position>0){
                order.quantity = td_position;
                order.offset = THOST_FTDC_OF_CloseToday;
                m_me->meSendOrder(&order);
            }
            if(yd_position>0)
            {
                order.quantity = yd_position;
                order.offset = THOST_FTDC_OF_CloseYesterday;
                m_me->meSendOrder(&order);
            }
        }
        else
        {
            order.offset = THOST_FTDC_OF_Close;
            m_me->meSendOrder(&order);
        }
    }


}

// 反向一笔持仓
void Tab_Account_Position::reverseOnePosition()
{

}

void Tab_Account_Position::initAccountTable()
{
    table_account = new QTableWidget();
    table_account->setAutoFillBackground(true);
    table_account->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_account->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_account->setParent(this);
    this->addTab(table_account, QString::fromLocal8Bit("账户资金"));

    name_account.empty();
    name_account.append(QString::fromLocal8Bit("账户"));
    name_account.append(QString::fromLocal8Bit("昨结"));
    name_account.append(QString::fromLocal8Bit("净值"));
    name_account.append(QString::fromLocal8Bit("可用"));
    name_account.append(QString::fromLocal8Bit("手续费"));
    name_account.append(QString::fromLocal8Bit("保证金"));
    name_account.append(QString::fromLocal8Bit("平仓盈亏"));
    name_account.append(QString::fromLocal8Bit("持仓盈亏"));
    name_account.append(QString::fromLocal8Bit("风险度(%)"));
    name_account.append(QString::fromLocal8Bit("接口"));
    table_account->setColumnCount(name_account.size());
    table_account->setHorizontalHeaderLabels(name_account);
    table_account->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    //    table_account->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_account->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_account->verticalHeader()->setHidden(true);
}

void Tab_Account_Position::initPositionTable()
{
    table_poistion = new QTableWidget();
    table_poistion->setAutoFillBackground(true);
    table_poistion->setStyleSheet("background-color: rgb( 69,69,69); color: white;border:1px solid #E0DDDC; gridline-color:red;  ");
    table_poistion->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_poistion->setParent(this);
    this->addTab(table_poistion, QString::fromLocal8Bit("当前持仓"));

    name_poistion.empty();
    name_poistion.append(QString::fromLocal8Bit("账户"));
    name_poistion.append(QString::fromLocal8Bit("合约代码"));
    name_poistion.append(QString::fromLocal8Bit("方 向"));
    name_poistion.append(QString::fromLocal8Bit("买 卖"));
    name_poistion.append(QString::fromLocal8Bit("持仓总量"));
    name_poistion.append(QString::fromLocal8Bit("今持仓"));
    name_poistion.append(QString::fromLocal8Bit("昨持仓"));
    name_poistion.append(QString::fromLocal8Bit("最新价"));
    name_poistion.append(QString::fromLocal8Bit("开仓价值"));
    name_poistion.append(QString::fromLocal8Bit("开仓均价"));
    name_poistion.append(QString::fromLocal8Bit("持仓盈亏"));
    name_poistion.append(QString::fromLocal8Bit("交易所"));
    name_poistion.append(QString::fromLocal8Bit("接口"));
    // name_poistion.append(QString::fromLocal8Bit("结算编号"));
    table_poistion->setColumnCount(name_poistion.size());
    table_poistion->setHorizontalHeaderLabels(name_poistion);
    table_poistion->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    table_poistion->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_poistion->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_poistion->verticalHeader()->setHidden(true);
}

void Tab_Account_Position::initLogTable()
{
    table_log = new QTableWidget();
    table_log->setAutoFillBackground(true);
    table_log->setStyleSheet("background-color: rgb( 69,69,69); color: white;border:1px solid #E0DDDC; gridline-color:red;  ");
    table_log->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_log->setParent(this);
    this->addTab(table_log, QString::fromLocal8Bit("日志"));

    name_log.empty();
    name_log.append(QString::fromLocal8Bit("时间"));
    name_log.append(QString::fromLocal8Bit("类型"));
    name_log.append(QString::fromLocal8Bit("信息"));
    table_log->setColumnCount(name_log.size());
    table_log->setHorizontalHeaderLabels(name_log);
    table_log->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    //    table_log->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_log->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_log->verticalHeader()->setHidden(true);
}

void Tab_Account_Position::positionTableOffset()
{
    menu_position = new QMenu(this->table_poistion);

    action_offset_position = new QAction(QString::fromLocal8Bit("平仓"), this);
    menu_position->addAction(action_offset_position);

    action_reverse_position = new QAction(QString::fromLocal8Bit("反向"), this);
    menu_position->addAction(action_reverse_position);

    this->table_poistion->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table_poistion, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotOffsetPosition(const QPoint&)));

    connect(action_offset_position, SIGNAL(triggered(bool)), this, SLOT(offsetOnePosition()));
    // connect
}
