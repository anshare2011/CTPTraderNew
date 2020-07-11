#include "klinechart.h"

KlineChart::KlineChart(QWidget *parent)
{
    this->setTabsClosable(false);
    this->setStyleSheet("color: darkCyan;font:10 '宋体';");
    this->klineInit();
    this->initFutureTable();
    this->initOptionTable();
    currentFrequency = tr("1分钟");
    // currentSymbol = "";

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(SLOTMainCanvas(QPoint)));

    connect(table_future, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(doubleClicked(int,int)));

    // 期权表头通知当前选中的合约 m_future_top
    connect(m_future_top, SIGNAL(sendSelectedOption(QString)), this, SLOT(receiveSelectedOption(QString)));

}

void KlineChart::klineInit()
{
    // 设置表的样式
    QString str = tr("QTabBar::tab{width: %1px;height:24px;font:12px;color:black}").arg(100);
    str.append("QTabWidget::pane{border-width: 0px;border-color:white;border-style:outset;background-color: rgb(69, 69, 69);border-radius: 3px;}");
    str.append("QTabBar::tab:selected{margin-left: 1;margin-right: 0;color: black;background-color:rbg(50, 100, 100);} ");
    str.append("QTabBar::tab:!selected{margin-left: 1;margin-right: 0;color: white;background-color:rbg(69, 69, 69);} ");
    str.append("QTabBar::tab:hover:!selected{color: white;margin-left: 1;margin-right: 0;background-color:rbg(100,200,100);} ");
    str.append("QTabBar::tab:!selected{margin-top: 3px;margin-bottom: -1px;} ");

    this->setStyleSheet(str);
    this->setDocumentMode(true);
}

void KlineChart::initFutureTable()
{    
    table_future = new QTableWidget();
    table_future->setAutoFillBackground(true);
    table_future->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_future->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_future->setParent(this);
    // this->addTab(table_future, future_name);

    QWidget *cornerWidget = new QWidget;
    QWidget *widget1 = new QWidget;                // 右边闲置空间
    widget1->setMinimumWidth(20);
    widget1->setMaximumWidth(20);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);         // 控件与窗体左右边距
    hlayout->setSpacing(10);       // 控件之间的边距
    hlayout->setDirection(QBoxLayout::LeftToRight);//设置布局方向
    hlayout->addWidget(table_future);
    hlayout->addWidget(widget1);
    cornerWidget->setLayout(hlayout);

    this->addTab(cornerWidget, future_name);

    name_future.empty();
    name_future.append(QString::fromLocal8Bit("序号"));
    name_future.append(QString::fromLocal8Bit("主力合约"));
    name_future.append(QString::fromLocal8Bit("合约名称"));
    name_future.append(QString::fromLocal8Bit("时间"));
    name_future.append(QString::fromLocal8Bit("最新价"));
    name_future.append(QString::fromLocal8Bit("涨跌"));
    name_future.append(QString::fromLocal8Bit("幅度(%)"));
    name_future.append(QString::fromLocal8Bit("卖一价"));
    name_future.append(QString::fromLocal8Bit("卖一量"));
    name_future.append(QString::fromLocal8Bit("买一价"));
    name_future.append(QString::fromLocal8Bit("买一量"));
    name_future.append(QString::fromLocal8Bit("成交量"));
    name_future.append(QString::fromLocal8Bit("持仓量"));
    name_future.append(QString::fromLocal8Bit("昨结价"));
    name_future.append(QString::fromLocal8Bit("昨收价"));
    name_future.append(QString::fromLocal8Bit("开盘价"));
    name_future.append(QString::fromLocal8Bit("最高价"));
    name_future.append(QString::fromLocal8Bit("最低价"));
    name_future.append(QString::fromLocal8Bit("涨停价"));
    name_future.append(QString::fromLocal8Bit("跌停价"));
    name_future.append(QString::fromLocal8Bit("交易所"));

    table_future->setColumnCount(name_future.size());
    table_future->setHorizontalHeaderLabels(name_future);
    table_future->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    table_future->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_future->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_future->verticalHeader()->setHidden(true);

}

void KlineChart::initOptionTable()
{
    table_option = new QTableWidget();
    table_option->setAutoFillBackground(true);
    table_option->setStyleSheet("background-color: rgb( 69,69,69); color: white; ");
    table_option->setStyleSheet("QHeaderView::section{background-color:rgb( 42, 42, 42); color: darkCyan;};");
    table_option->setParent(this);

    // this->addTab(table_option, option_name);
    QWidget *cornerWidget = new QWidget;
    m_future_top = new FutureTableTop();
    m_future_top->setMinimumHeight(80);
    m_future_top->setMaximumHeight(80);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);         // 控件与窗体左右边距
    vlayout->setSpacing(0);        // 控件之间的边距
    vlayout->setDirection(QBoxLayout::TopToBottom);//设置布局方向
    vlayout->addWidget(m_future_top);
    vlayout->addWidget(table_option);
    cornerWidget->setLayout(vlayout);
    this->addTab(cornerWidget, option_name);

    name_option.empty();
    name_option.append(QString::fromLocal8Bit("理论价值"));
    name_option.append(QString::fromLocal8Bit("内在价值"));
    name_option.append(QString::fromLocal8Bit("时间价值"));
    name_option.append(QString::fromLocal8Bit("成交量"));
    name_option.append(QString::fromLocal8Bit("涨跌幅"));
    name_option.append(QString::fromLocal8Bit("买价"));
    name_option.append(QString::fromLocal8Bit("买量"));
    name_option.append(QString::fromLocal8Bit("卖价"));
    name_option.append(QString::fromLocal8Bit("卖量"));
    name_option.append(QString::fromLocal8Bit("最新价"));
    name_option.append(QString::fromLocal8Bit("C"));
    name_option.append(QString::fromLocal8Bit("执行价"));
    name_option.append(QString::fromLocal8Bit("P"));
    name_option.append(QString::fromLocal8Bit("最新价"));
    name_option.append(QString::fromLocal8Bit("买价"));
    name_option.append(QString::fromLocal8Bit("买量"));
    name_option.append(QString::fromLocal8Bit("卖价"));
    name_option.append(QString::fromLocal8Bit("卖量"));
    name_option.append(QString::fromLocal8Bit("涨跌幅"));
    name_option.append(QString::fromLocal8Bit("成交量"));
    name_option.append(QString::fromLocal8Bit("时间价值"));
    name_option.append(QString::fromLocal8Bit("内在价值"));
    name_option.append(QString::fromLocal8Bit("理论价值"));
    table_option->setColumnCount(name_option.size());
    table_option->setHorizontalHeaderLabels(name_option);
    table_option->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //Stretch ResizeToContents);  //将表头显示在表内
    table_option->setSelectionBehavior(QAbstractItemView::SelectRows);             // 选中一行
    table_option->setEditTriggers(QAbstractItemView::NoEditTriggers);              // 表格不可编辑
    table_option->verticalHeader()->setHidden(true);
}

void KlineChart::setCurrentFrequency(QString frequency)
{
    //    this->currentFrequency = frequency;

    //    // 主界面没有1个K线表
    //    if(current_symbol.size() <1)        //防止样本初始为空时下面查找异常
    //        return;

    //    if(false == all_klineWidget.contains(currentSymbol))
    //        return;

    //    auto iter = all_instruments.find(currentSymbol);
    //    if(iter != all_instruments.end())
    //    {
    //        KlineWidget* kline = all_klineWidget[currentSymbol];
    //        OneSymbolKlineData * data = all_symbol_data[currentSymbol];
    //        kline->updateKline(currentFrequency, data);
    //    }
}

void KlineChart::addWidget1(QString symbol, QWidget *w)
{
    int index = this->addTab(w, symbol);
    this->setCurrentIndex(index);
}

void KlineChart::clearOptionTable()
{
    while(table_option->rowCount() > 0)
    {
        table_option->removeRow(0);
    }
}

void KlineChart::setCurrentSymbol(QString symbol)
{
    //    this->currentSymbol = symbol;
    //    if(false == all_klineWidget.contains(currentSymbol))
    //        return;

    //    KlineWidget* kline = all_klineWidget[currentSymbol];
    //    OneSymbolKlineData * data = all_symbol_data[currentSymbol];
    //    kline->updateKline(currentFrequency, data);
}

void KlineChart::receiveKlineInformation(QString symbol, OneSymbolKlineData *data)
{
    if(!all_instruments.contains(symbol))
    {
        int x = this->width();
        int y = this->height();
        KlineWidget* kline = new KlineWidget(x, y);
        this->addWidget1(symbol, kline);
        all_instruments.insert(symbol);
        all_klineWidget.insert(symbol, kline);
    }
    KlineWidget* kline = all_klineWidget[symbol];
    kline->updateKline(data);

    // 将数据指针保存在本地
    all_symbol_data[symbol] = data;
}

void KlineChart::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    QString symbol = quote->symbol;
    if(all_klineWidget.contains(symbol))
    {
        KlineWidget* kline = all_klineWidget[symbol];
        kline->receiveQuoteInformation(quote);                      // K线下单板更新行情
    }

    if(current_symbol == this->option_name)
    {
        m_future_top->receiveQuoteInformation(quote);               // 期权页接收行情

        QString symbil_mid = symbol.mid(2);
        bool is_option = (symbil_mid.contains("P") || symbil_mid.contains("C"));
        if(is_option)
            this->receiveOptionQuoteInformation(quote);
        return;
    }



    if(!m_me->getMainInstrument().contains(symbol)) return;             // 只显示主力
    int openInterest = quote->openInterest;                             // 只显示持仓大于4000的合约
    if(openInterest < 4000)    return;

    double change = (quote->lastPrice - quote->preSettlementPrice);
    double changePercent = (quote->lastPrice - quote->preSettlementPrice)*100 /quote->preSettlementPrice;

    if(symbol_row.contains(symbol)){
        //         if(quote_count++ % cycle_num != 0)  return;      // 全量刷新会使页面卡死

        int row = symbol_row[symbol];
        QScrollBar * verticalScroll = table_future->verticalScrollBar();
        if(verticalScroll->maximum() > 0)   // 根据隐藏的行数判断，隐藏不显示的不刷新
        {
            if (row < verticalScroll->value()) return;
            if (row > verticalScroll->value() + future_row_count - verticalScroll->maximum()) return;
        }

        table_future->item(row, 3)->setText(quote->time);
        table_future->item(row, 4)->setText(QString::number(quote->lastPrice));
        table_future->item(row, 5)->setText(QString::number(change));
        table_future->item(row, 6)->setText(QString::number(changePercent, 'f', 2) + QString("%"));
        if(changePercent > 0)
        {
            if(table_future->item(row, 6)->backgroundColor() != Qt::darkRed)
            {
                table_future->item(row, 6)->setBackgroundColor(Qt::darkRed);
            }
        }
        else if(changePercent < 0)
        {
            if(table_future->item(row, 6)->backgroundColor() != Qt::darkGreen)
            {
                table_future->item(row, 6)->setBackgroundColor(Qt::darkGreen);
            }
        }

        table_future->item(row, 7)->setText(QString::number(quote->askPrice1));
        table_future->item(row, 8)->setText(QString::number(quote->askVolume1));
        table_future->item(row, 9)->setText(QString::number(quote->bidPrice1));
        table_future->item(row, 10)->setText(QString::number(quote->bidVolume1));
        table_future->item(row, 11)->setText(QString::number(quote->volume));
        table_future->item(row, 12)->setText(QString::number(quote->openInterest));
        table_future->item(row, 16)->setText(QString::number(quote->highPrice));
        table_future->item(row, 17)->setText(QString::number(quote->lowPrice));
    }
    else{
        int row =table_future->rowCount();
        symbol_row[symbol] = row;
        table_future->insertRow(row);
        future_row_count +=1;    // 插入一行，行数加一
        table_future->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
        table_future->setItem(row, 1, new QTableWidgetItem(symbol));
        table_future->setItem(row, 2, new QTableWidgetItem(quote->symbol_name));
        table_future->setItem(row, 3, new QTableWidgetItem(quote->time));
        table_future->setItem(row, 4, new QTableWidgetItem(QString::number(quote->lastPrice)));
        table_future->setItem(row, 5, new QTableWidgetItem(QString::number(quote->lastPrice - quote->preSettlementPrice)));
        table_future->setItem(row, 6, new QTableWidgetItem(QString::number(changePercent, 'f', 2) + QString("%")));
        table_future->setItem(row, 7, new QTableWidgetItem(QString::number(quote->askPrice1)));
        table_future->setItem(row, 8, new QTableWidgetItem(QString::number(quote->askVolume1)));
        table_future->setItem(row, 9, new QTableWidgetItem(QString::number(quote->bidPrice1)));
        table_future->setItem(row, 10, new QTableWidgetItem(QString::number(quote->bidVolume1)));
        table_future->setItem(row, 11, new QTableWidgetItem(QString::number(quote->volume)));
        table_future->setItem(row, 12, new QTableWidgetItem(QString::number(quote->openInterest)));
        table_future->setItem(row, 13, new QTableWidgetItem(QString::number(quote->preSettlementPrice)));
        table_future->setItem(row, 14, new QTableWidgetItem(QString::number(quote->preClosePrice)));
        table_future->setItem(row, 15, new QTableWidgetItem(QString::number(quote->openPrice)));
        table_future->setItem(row, 16, new QTableWidgetItem(QString::number(quote->highPrice)));
        table_future->setItem(row, 17, new QTableWidgetItem(QString::number(quote->lowPrice)));
        table_future->setItem(row, 18, new QTableWidgetItem(QString::number(quote->upperLimit)));
        table_future->setItem(row, 19, new QTableWidgetItem(QString::number(quote->lowerLimit)));
        table_future->setItem(row, 20, new QTableWidgetItem(quote->exchangeid));

        // 居中
        for(int i = 0; i< name_future.size(); ++i)
        {
            table_future->item(row, i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            if(i == 15)   // 开盘价，与昨天结算价进行比较
            {
                if(quote->openPrice >= quote->preSettlementPrice)
                    table_future->item(row, i)->setForeground(QColor(255, 0, 0));
                else
                    table_future->item(row, i)->setForeground(QColor(0, 255, 0));
            }
            else if(i == 18)    // 涨停价
            {
                table_future->item(row, i)->setForeground(QColor(255, 0, 0));
            }
            else if(i == 19)    // 跌停价
            {
                table_future->item(row, i)->setForeground(QColor(0, 255, 0));
            }
            else{
                table_future->item(row, i)->setForeground(QColor(222, 221, 140));    // 218, 165, 32
            }
        }
    }

}

void KlineChart::SLOTMainCanvas(QPoint)
{
    qDebug()<< "KlineChart::SLOTMainCanvas(QPoint)";
    QMenu *menu = new QMenu();
    QAction *pnew = new QAction("select_point", this);
    connect(pnew, SIGNAL(triggered(bool)), this, SLOT(slot_myfun()));
    menu->addAction(pnew);
    menu->move(this->cursor().pos());
    menu->show();
}

void KlineChart::slot_myfun()
{
    ;
}

void KlineChart::doubleClicked(int row, int column)
{
    QTableWidgetItem * item1 = new QTableWidgetItem;
    item1 = table_future->item(row, 1);
    QString instrument = item1->text();
    m_me->meSubscribeInstrument(instrument);

    // 自动切换到刚订阅的表(表已经存在则循环跳转，不存在会自动跳转)
    for(int i = 0; i< this->count(); ++i)
    {
        QString tab_name = this->tabText(i);
        if(tab_name == instrument)
            this->setCurrentIndex(i);
    }
}

void KlineChart::currentIndexChanged(int index)
{
    //    this->setCurrentIndex(index);
    //    int current_index = this->currentIndex();
    //    currentSymbol = this->tabText(current_index);

    //    QString send_name = "";
    //    if(-1 == current_index || currentSymbol == future_name || currentSymbol == option_name)
    //    {
    //        send_name = "";
    //    }
    //    else
    //    {
    //        send_name = currentSymbol;
    //    }
    //    // 通知tradeboard 标的已经变化
    //    qDebug()<< "KlineChart::currentIndexChanged(int index)"<< currentSymbol;
    //    emit sendCurrentSymbolToTradeboard(send_name);
}

void KlineChart::receiveSelectedOption(QString symbol)
{
    //    QMap<QString, OptionInfo> map_symbol_optioninfo;                //  合约：期权信息 ru2013P9300:[]
    //    QMap<QString, QList<QString>> map_symbol_optionlist;            //  ru2013:[ru2013P9300, ru2103P14750]
    //    QMap<QString, QList<double>> map_symbol_StrikePricelist;        //  ru2013:行权价增序
    //    QStringList m_option_list;

    if(m_me->map_symbol_optionlist.contains(symbol))
    {
        this->clearOptionTable();
        QList<double>& priceList = m_me->map_symbol_StrikePricelist[symbol];
        foreach(double price, priceList)
        {
            int row =table_option->rowCount();
            table_option->insertRow(row);
            table_option->setItem(row, 10, new QTableWidgetItem(QString("C")));
            table_option->setItem(row, 11, new QTableWidgetItem(QString::number(price)));
            table_option->setItem(row, 12, new QTableWidgetItem(QString("P")));
            // 期权表居中
            for(int i = 0; i< name_option.size(); ++i)
            {
                // table_option->item(row, i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            }
        }
    }

}

void KlineChart::receiveQuickType(QuickType type)
{
    qDebug()<< "KlineChart::receiveQuickType QuickType:"<< type;
    if(all_klineWidget.contains(current_symbol))
    {
        all_klineWidget[current_symbol]->receiveQuickType(type);
    }
    else
    {
        qDebug()<< QString::fromLocal8Bit("未选中合约，快捷交易无效");
    }
}

void KlineChart::receiveOptionQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    for(int i = 0; i< table_option->rowCount(); ++i)
    {
        QString strikePrice = table_option->item(i, 11)->text();
        QString cStrikePrice = QString("C") + strikePrice;
        QString pStrikePrice = QString("P") + strikePrice;

        double future_lastprice = m_future_top->getLastPrice();

        if(quote->symbol.contains(cStrikePrice))
        {
            qDebug()<< "KlineChart::receiveOptionQuoteInformation"<< "c"<< future_lastprice<< strikePrice;
            double inside_value = future_lastprice > strikePrice.toDouble() ? future_lastprice - strikePrice.toDouble() : 0 ;
            double changePercent = (quote->lastPrice - quote->preSettlementPrice) *100 / quote->preSettlementPrice;
            if(future_lastprice == 0) inside_value = 0;

            table_option->setItem(i, 1, new QTableWidgetItem(QString::number(inside_value)));
            table_option->setItem(i, 2, new QTableWidgetItem(QString::number(quote->lastPrice - inside_value)));
            table_option->setItem(i, 3, new QTableWidgetItem(QString::number(quote->volume)));
            table_option->setItem(i, 4, new QTableWidgetItem(QString::number(changePercent, 'f', 2) + QString("%")));
            table_option->setItem(i, 5, new QTableWidgetItem(QString::number(quote->bidPrice1)));
            table_option->setItem(i, 6, new QTableWidgetItem(QString::number(quote->bidVolume1)));
            table_option->setItem(i, 7, new QTableWidgetItem(QString::number(quote->askPrice1)));
            table_option->setItem(i, 8, new QTableWidgetItem(QString::number(quote->askVolume1)));
            table_option->setItem(i, 9, new QTableWidgetItem(QString::number(quote->lastPrice)));
            table_option->setItem(i, 10, new QTableWidgetItem(quote->symbol));
        }
        if(quote->symbol.contains(pStrikePrice))
        {
            double changePercent = (quote->lastPrice - quote->preSettlementPrice) *100 / quote->preSettlementPrice;
            double inside_value = strikePrice.toDouble() > future_lastprice ? strikePrice.toDouble() - future_lastprice : 0 ;
            if(future_lastprice == 0) inside_value = 0;

            table_option->setItem(i, 12, new QTableWidgetItem(quote->symbol));
            table_option->setItem(i, 13, new QTableWidgetItem(QString::number(quote->lastPrice)));
            table_option->setItem(i, 14, new QTableWidgetItem(QString::number(quote->bidPrice1)));
            table_option->setItem(i, 15, new QTableWidgetItem(QString::number(quote->bidVolume1)));
            table_option->setItem(i, 16, new QTableWidgetItem(QString::number(quote->askPrice1)));
            table_option->setItem(i, 17, new QTableWidgetItem(QString::number(quote->askVolume1)));
            table_option->setItem(i, 18, new QTableWidgetItem(QString::number(changePercent, 'f', 2) + QString("%")));
            table_option->setItem(i, 19, new QTableWidgetItem(QString::number(quote->volume)));
            table_option->setItem(i, 20, new QTableWidgetItem(QString::number(quote->lastPrice - inside_value)));
            table_option->setItem(i, 21, new QTableWidgetItem(QString::number(inside_value)));
        }
    }
}

void KlineChart::paintEvent(QPaintEvent *event)
{
    //    qDebug()<< "KlineChart::paintEvent";
    //    QPainter painter(this);
    //    painter.setPen(QPen(Qt::red, 1));
    //    painter.drawLine(10, 10, this->width() - 1, 10);
    //    painter.drawLine(10, 10, 10, this->height() - 1);
    //    painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height() - 1);
    //    painter.drawLine(0, this->height() - 1, this->width() - 1, this->height() - 1);
}

void KlineChart::mousePressEvent(QMouseEvent *event)
{
    qDebug()<< "KlineChart::mousePressEvent";
    //    if(event->button() == Qt::RightButton)
    {
        table_future->clearFocus();
        table_future->setCurrentItem(NULL);    // 取消选中状态
    }
}

