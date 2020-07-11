#include "tradeboard.h"

Tradeboard::Tradeboard(QWidget *parent) : QWidget(parent)
{
    this->setMinimumWidth(400);
    // 初始化控件
    this->tradeboardInit();
    m_winLoss = new SetStopWinLoss();
    m_condition = new SetConditionOrder();

    this->addConnect();
}

Tradeboard::~Tradeboard()
{
    delete m_winLoss;
    m_winLoss = nullptr;
    delete m_condition;
    m_condition = nullptr;
}

BoardInfo Tradeboard::getBoardInfo()
{
    BoardInfo board;
    board.instrument = tradeboard.instrument_show->text();
    board.ask1price = tradeboard.AskPrice1_show->text().toDouble();
    board.bid1price = tradeboard.BidPrice1_show->text().toDouble();
    board.upperprice = tradeboard.upperPrice_show->text().toDouble();
    board.lowerprice = tradeboard.lowerPrice_show->text().toDouble();
    board.userid = tradeboard.account_com->currentText();
    return board;
}

void Tradeboard::receiveQuickType(QuickType type)
{
    orderCommonRequest order_info;

        order_info.userid = QString("ALL");     //tradeboard.account_com->currentText();   //从界面拿下单id
        order_info.instrument = tradeboard.instrument_show->text();
        order_info.quantity  = 1;
        order_info.pricetype = THOST_FTDC_OPT_LimitPrice;
        order_info.type = THOST_FTDC_CIDT_Speculation;
        order_info.localType = OrderLocalType::quick;
        order_info.exchangeid = m_me->getExchangeid(order_info.instrument);

        if(type == QuickType::QuickOpenBuy)
        {
            order_info.offset = THOST_FTDC_OF_Open;
            order_info.direction = THOST_FTDC_D_Buy;
            order_info.price = tradeboard.upperPrice_show->text().toDouble();
        }
        else if(type == QuickType::QuickOpenSell)
        {
            order_info.offset = THOST_FTDC_OF_Open;
            order_info.direction = THOST_FTDC_D_Sell;
            order_info.price = tradeboard.lowerPrice_show->text().toDouble();
        }
        else if(type == QuickType::QuickOffsetBuy)
        {
            order_info.offset = THOST_FTDC_OF_Close;
            order_info.direction = THOST_FTDC_D_Buy;
            order_info.price = tradeboard.upperPrice_show->text().toDouble();
        }
        else if(type == QuickType::QuickOffsetSell)
        {
            order_info.offset = THOST_FTDC_OF_Close;
            order_info.direction = THOST_FTDC_D_Sell;
            order_info.price = tradeboard.lowerPrice_show->text().toDouble();
        }
        QString result = m_me->meSendOrder(&order_info);
}

void Tradeboard::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    // 更新价格框的价格
    if(tradeboard.pricetype_com->currentIndex() == 0)
    {
        // 限价
    }
    else if(tradeboard.pricetype_com->currentIndex() == 1)
    {
        // 最新价
        tradeboard.instrumentPrice_spin->setValue(quote->lastPrice);
    }
    else if(tradeboard.pricetype_com->currentIndex() == 2)
    {
        // 对手价
        if(tradeboard.direction_com->currentIndex() ==0)
            tradeboard.instrumentPrice_spin->setValue(quote->askPrice1);
        else
            tradeboard.instrumentPrice_spin->setValue(quote->bidPrice1);
    }
    else if(tradeboard.pricetype_com->currentIndex() == 3)
    {
        // 市价
        if(tradeboard.direction_com->currentIndex() ==0)
            tradeboard.instrumentPrice_spin->setValue(quote->upperLimit);
        else
            tradeboard.instrumentPrice_spin->setValue(quote->lowPrice);
    }


    // 涨幅
    double changePercent = 0;
    // 振幅
    double zhendongPercent = 0;
    if(quote->preSettlementPrice > 0.00000001)
    {
        changePercent = (quote->lastPrice - quote->preSettlementPrice)*100 /quote->preSettlementPrice;
        zhendongPercent = (quote->highPrice - quote->lowPrice)*100 /quote->preSettlementPrice;
    }

    // 全量刷新
    tradeboard.instrument_show->setText(quote->symbol);
    tradeboard.AskPrice1_show->setText(QString::number(quote->askPrice1));
    tradeboard.AskPrice1_vol_show->setText(QString::number(quote->askVolume1));

    tradeboard.BidPrice1_show->setText(QString::number(quote->bidPrice1));
    tradeboard.BidPrice1_vol_show->setText(QString::number(quote->bidVolume1));

    tradeboard.lastPrice_show->setText(QString::number(quote->lastPrice));
    tradeboard.preSettlementPrice_show->setText(QString::number(quote->preSettlementPrice));

    tradeboard.zhangDie_show->setText(QString::number(quote->lastPrice - quote->preSettlementPrice));
    tradeboard.zuoshouPrice_show->setText(QString::number(quote->preClosePrice));

    tradeboard.fuDu_show->setText(QString::number(changePercent, 'f',2));
    tradeboard.openPrice_show->setText(QString::number(quote->openPrice));

    tradeboard.zhenFu_show->setText(QString::number(zhendongPercent, 'f',2));
    tradeboard.highPrice_show->setText(QString::number(quote->highPrice));

    tradeboard.dealAmount_show->setText(QString::number(quote->volume));
    tradeboard.lowPrice_show->setText(QString::number(quote->lowPrice));

    tradeboard.holdingAmount_show->setText(QString::number(quote->openInterest));
    tradeboard.upperPrice_show->setText(QString::number(quote->upperLimit));

    tradeboard.timenow_show->setText(quote->time);
    tradeboard.lowerPrice_show->setText(QString::number(quote->lowerLimit));

    // 改变颜色
    //askprice1
    if(quote->askPrice1 > quote->preSettlementPrice && tradeboard.AskPrice1_show->styleSheet().contains("green"))
    {
        tradeboard.AskPrice1_show->setStyleSheet("color: red;");
    }
    else if(quote->askPrice1 < quote->preSettlementPrice && tradeboard.AskPrice1_show->styleSheet().contains("red"))
    {
        tradeboard.AskPrice1_show->setStyleSheet("color: green;");
    }

    //bidPrice1
    if(quote->bidPrice1 > quote->preSettlementPrice && tradeboard.BidPrice1_show->styleSheet().contains("green"))
    {
        tradeboard.BidPrice1_show->setStyleSheet("color: red;");
    }
    else if(quote->bidPrice1 < quote->preSettlementPrice && tradeboard.BidPrice1_show->styleSheet().contains("red"))
    {
        tradeboard.BidPrice1_show->setStyleSheet("color: green;");
    }

    //lastPrice and zhangDie
    if(quote->lastPrice > quote->preSettlementPrice)
    {
        if(tradeboard.lastPrice_show->styleSheet().contains("green"))
            tradeboard.lastPrice_show->setStyleSheet("color: red;");
        if(tradeboard.zhangDie_show->styleSheet().contains("green"))
            tradeboard.zhangDie_show->setStyleSheet("color: red;");
        if(tradeboard.fuDu_show->styleSheet().contains("green"))
            tradeboard.fuDu_show->setStyleSheet("color: red;");
    }
    else if(quote->lastPrice < quote->preSettlementPrice)
    {
        if(tradeboard.lastPrice_show->styleSheet().contains("red"))
            tradeboard.lastPrice_show->setStyleSheet("color: green;");
        if(tradeboard.zhangDie_show->styleSheet().contains("red"))
            tradeboard.zhangDie_show->setStyleSheet("color: green;");
        if(tradeboard.fuDu_show->styleSheet().contains("red"))
            tradeboard.fuDu_show->setStyleSheet("color: green;");
    }
}

void Tradeboard::changeInstrument(QString symbol)
{
    tradeboard.instrument_show->setText(symbol);

    tradeboard.AskPrice1_show->setText("0");
    tradeboard.AskPrice1_vol_show->setText("0");

    tradeboard.BidPrice1_show->setText("0");
    tradeboard.BidPrice1_vol_show->setText("0");

    tradeboard.lastPrice_show->setText("0");
    tradeboard.preSettlementPrice_show->setText("0");

    tradeboard.zhangDie_show->setText("0");
    tradeboard.zuoshouPrice_show->setText("0");

    tradeboard.fuDu_show->setText("0");
    tradeboard.openPrice_show->setText("0");

    tradeboard.zhenFu_show->setText("0");
    tradeboard.highPrice_show->setText("0");

    tradeboard.dealAmount_show->setText("0");
    tradeboard.lowPrice_show->setText("0");

    tradeboard.holdingAmount_show->setText("0");
    tradeboard.upperPrice_show->setText("0");

    tradeboard.timenow_show->setText("0");
    tradeboard.lowerPrice_show->setText("0");
}

void Tradeboard::addAccount(QString account)
{
    tradeboard.account_com->addItem(account);
}

void Tradeboard::delAccount(QString account)
{
    int index = tradeboard.account_com->findText(account);
    if(-1 == index)
        return;
    tradeboard.account_com->removeItem(index);
}

void Tradeboard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::darkCyan, 1));
    painter.drawLine(0, 0, this->width() - 1, 0);
    painter.drawLine(0, 0, 0, this->height() - 1);
    painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height() - 1);
    painter.drawLine(0, this->height() - 1, this->width() - 1, this->height() - 1);

    QPainter painter1(this);
    painter1.setPen(QPen(Qt::gray, 1));               // Qt::red
    painter1.drawLine(1, 30, this->width() - 1, 30);
    painter1.drawLine(1, 110, this->width() - 1, 110);
    painter1.drawLine(1, 330, this->width() - 1, 330);
    painter1.drawLine(200, 110, 200, 330);
}

void Tradeboard::tradeboardInit()
{
    int height_0 = 0;
    QFont ft12("宋体", 12, QFont::Normal);
    QFont ft16("Times", 18, QFont::Bold);

    // 1
    tradeboard.instrument_info = new QLabel(QString::fromLocal8Bit("<u>合约信息</u>"),this);
    tradeboard.instrument_info->setGeometry(begin_1, 6, 80, 24);
    tradeboard.instrument_info->setAlignment(Qt::AlignLeft);
    tradeboard.instrument_info->setFont(ft12);
    tradeboard.instrument_info->installEventFilter(this);
    tradeboard.instrument_show = new QLabel(QString::fromLocal8Bit(""),this);
    tradeboard.instrument_show->setGeometry(begin_2, 2, 180, 28);
    tradeboard.instrument_show->setAlignment(Qt::AlignCenter);
    tradeboard.instrument_show->setFont(ft16);

    // 2
    height_0 += 40;
    tradeboard.AskPrice1 = new QLabel(QString::fromLocal8Bit("卖一"),this);
    tradeboard.AskPrice1->setGeometry(begin_1, height_0, 70, height_1);
    tradeboard.AskPrice1->setAlignment(Qt::AlignLeft);
    tradeboard.AskPrice1->setFont(ft16);
    tradeboard.AskPrice1_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.AskPrice1_show->setGeometry(90, height_0, 100, height_1);
    tradeboard.AskPrice1_show->setAlignment(Qt::AlignRight);
    tradeboard.AskPrice1_show->setFont(ft16);
    tradeboard.AskPrice1_show->setStyleSheet("color: red;");
    tradeboard.AskPrice1_vol_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.AskPrice1_vol_show->setGeometry(begin_3, height_0, 80, height_1);
    tradeboard.AskPrice1_vol_show->setAlignment(Qt::AlignLeft);
    tradeboard.AskPrice1_vol_show->setFont(ft16);
    tradeboard.AskPrice1_vol_show->setStyleSheet("color: yellow;");

    // 3
    height_0 += 40;
    tradeboard.BidPrice1 = new QLabel(QString::fromLocal8Bit("买一"),this);
    tradeboard.BidPrice1->setGeometry(begin_1, height_0, 70, height_1);
    tradeboard.BidPrice1->setAlignment(Qt::AlignLeft);
    tradeboard.BidPrice1->setFont(ft16);
    tradeboard.BidPrice1_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.BidPrice1_show->setGeometry(90, height_0, 100, height_1);
    tradeboard.BidPrice1_show->setAlignment(Qt::AlignRight);
    tradeboard.BidPrice1_show->setFont(ft16);
    tradeboard.BidPrice1_show->setStyleSheet("color: red;");
    tradeboard.BidPrice1_vol_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.BidPrice1_vol_show->setGeometry(begin_3, height_0, 80, height_1);
    tradeboard.BidPrice1_vol_show->setAlignment(Qt::AlignLeft);
    tradeboard.BidPrice1_vol_show->setFont(ft16);
    tradeboard.BidPrice1_vol_show->setStyleSheet("color: yellow;");

    // 4
    height_0 += 40;
    tradeboard.lastPrice = new QLabel(QString::fromLocal8Bit("最新价"),this);
    tradeboard.lastPrice->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.lastPrice->setAlignment(Qt::AlignLeft);
    tradeboard.lastPrice->setFont(ft12);
    tradeboard.lastPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.lastPrice_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.lastPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.lastPrice_show->setFont(ft12);
    tradeboard.lastPrice_show->setStyleSheet("color: red;");
    tradeboard.preSettlementPrice = new QLabel(QString::fromLocal8Bit("昨结价"),this);
    tradeboard.preSettlementPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.preSettlementPrice->setAlignment(Qt::AlignLeft);
    tradeboard.preSettlementPrice->setFont(ft12);
    tradeboard.preSettlementPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.preSettlementPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.preSettlementPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.preSettlementPrice_show->setFont(ft12);
    tradeboard.preSettlementPrice_show->setStyleSheet("color: white;");

    // 5
    height_0 += 30;
    tradeboard.zhangDie = new QLabel(QString::fromLocal8Bit("涨跌"),this);
    tradeboard.zhangDie->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.zhangDie->setAlignment(Qt::AlignLeft);
    tradeboard.zhangDie->setFont(ft12);
    tradeboard.zhangDie_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.zhangDie_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.zhangDie_show->setAlignment(Qt::AlignRight);
    tradeboard.zhangDie_show->setFont(ft12);
    tradeboard.zhangDie_show->setStyleSheet("color: red;");
    tradeboard.zuoshouPrice = new QLabel(QString::fromLocal8Bit("昨收价"),this);
    tradeboard.zuoshouPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.zuoshouPrice->setAlignment(Qt::AlignLeft);
    tradeboard.zuoshouPrice->setFont(ft12);
    tradeboard.zuoshouPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.zuoshouPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.zuoshouPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.zuoshouPrice_show->setFont(ft12);
    tradeboard.zuoshouPrice_show->setStyleSheet("color: yellow;");

    // 6
    height_0 += 30;
    tradeboard.fuDu = new QLabel(QString::fromLocal8Bit("幅度(%)"),this);
    tradeboard.fuDu->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.fuDu->setAlignment(Qt::AlignLeft);
    tradeboard.fuDu->setFont(ft12);
    tradeboard.fuDu_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.fuDu_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.fuDu_show->setAlignment(Qt::AlignRight);
    tradeboard.fuDu_show->setFont(ft12);
    tradeboard.fuDu_show->setStyleSheet("color: red;");
    tradeboard.openPrice = new QLabel(QString::fromLocal8Bit("开盘价"),this);
    tradeboard.openPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.openPrice->setAlignment(Qt::AlignLeft);
    tradeboard.openPrice->setFont(ft12);
    tradeboard.openPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.openPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.openPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.openPrice_show->setFont(ft12);
    tradeboard.openPrice_show->setStyleSheet("color: red;");

    // 7
    height_0 += 30;
    tradeboard.zhenFu = new QLabel(QString::fromLocal8Bit("振幅(%)"),this);
    tradeboard.zhenFu->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.zhenFu->setAlignment(Qt::AlignLeft);
    tradeboard.zhenFu->setFont(ft12);
    tradeboard.zhenFu_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.zhenFu_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.zhenFu_show->setAlignment(Qt::AlignRight);
    tradeboard.zhenFu_show->setFont(ft12);
    tradeboard.zhenFu_show->setStyleSheet("color: yellow;");
    tradeboard.highPrice = new QLabel(QString::fromLocal8Bit("最高价"),this);
    tradeboard.highPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.highPrice->setAlignment(Qt::AlignLeft);
    tradeboard.highPrice->setFont(ft12);
    tradeboard.highPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.highPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.highPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.highPrice_show->setFont(ft12);
    tradeboard.highPrice_show->setStyleSheet("color: red;");

    // 8
    height_0 += 30;
    tradeboard.dealAmount = new QLabel(QString::fromLocal8Bit("成交量"),this);
    tradeboard.dealAmount->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.dealAmount->setAlignment(Qt::AlignLeft);
    tradeboard.dealAmount->setFont(ft12);
    tradeboard.dealAmount_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.dealAmount_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.dealAmount_show->setAlignment(Qt::AlignRight);
    tradeboard.dealAmount_show->setFont(ft12);
    tradeboard.dealAmount_show->setStyleSheet("color: yellow;");
    tradeboard.lowPrice = new QLabel(QString::fromLocal8Bit("最低价"),this);
    tradeboard.lowPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.lowPrice->setAlignment(Qt::AlignLeft);
    tradeboard.lowPrice->setFont(ft12);
    tradeboard.lowPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.lowPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.lowPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.lowPrice_show->setFont(ft12);
    tradeboard.lowPrice_show->setStyleSheet("color: green;");

    // 9
    height_0 += 30;
    tradeboard.holdingAmount = new QLabel(QString::fromLocal8Bit("持仓量"),this);
    tradeboard.holdingAmount->setGeometry(begin_1, height_0, 80, height_4);
    tradeboard.holdingAmount->setAlignment(Qt::AlignLeft);
    tradeboard.holdingAmount->setFont(ft12);
    tradeboard.holdingAmount_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.holdingAmount_show->setGeometry(begin_2, height_0, 80, height_4);
    tradeboard.holdingAmount_show->setAlignment(Qt::AlignRight);
    tradeboard.holdingAmount_show->setFont(ft12);
    tradeboard.holdingAmount_show->setStyleSheet("color: yellow;");
    tradeboard.upperPrice = new QLabel(QString::fromLocal8Bit("涨停价"),this);
    tradeboard.upperPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.upperPrice->setAlignment(Qt::AlignLeft);
    tradeboard.upperPrice->setFont(ft12);
    tradeboard.upperPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.upperPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.upperPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.upperPrice_show->setFont(ft12);
    tradeboard.upperPrice_show->setStyleSheet("color: red;");

    // 10
    height_0 += 30;
    tradeboard.timenow = new QLabel(QString::fromLocal8Bit("时间"),this);
    tradeboard.timenow->setGeometry(begin_1, height_0, 60, height_4);
    tradeboard.timenow->setAlignment(Qt::AlignLeft);
    tradeboard.timenow->setFont(ft12);
    tradeboard.timenow_show = new QLabel(QString::fromLocal8Bit("-"),this);
    tradeboard.timenow_show->setGeometry(80, height_0, 110, height_4);                    // 让时间框更宽
    tradeboard.timenow_show->setAlignment(Qt::AlignRight);
    tradeboard.timenow_show->setFont(ft12);
    tradeboard.lowerPrice = new QLabel(QString::fromLocal8Bit("跌停价"),this);
    tradeboard.lowerPrice->setGeometry(begin_3, height_0, 80, height_4);
    tradeboard.lowerPrice->setAlignment(Qt::AlignLeft);
    tradeboard.lowerPrice->setFont(ft12);
    tradeboard.lowerPrice_show = new QLabel(QString::fromLocal8Bit("0"),this);
    tradeboard.lowerPrice_show->setGeometry(begin_4, height_0, 80, height_4);
    tradeboard.lowerPrice_show->setAlignment(Qt::AlignRight);
    tradeboard.lowerPrice_show->setFont(ft12);
    tradeboard.lowerPrice_show->setStyleSheet("color: green;");

    // 11
    height_0 += 40;
    tradeboard.account = new QLabel(QString::fromLocal8Bit("账户"),this);
    tradeboard.account->setGeometry(begin_1, height_0, 40, height_4);
    tradeboard.account->setAlignment(Qt::AlignLeft);
    tradeboard.account->setFont(ft12);
    tradeboard.account_com = new QComboBox(this);
    tradeboard.account_com->addItem(QString::fromLocal8Bit("ALL"));
    tradeboard.account_com->setGeometry(60, height_0, 120, height_4);
    tradeboard.account_com->setFont(ft12);
    tradeboard.price = new QLabel(QString::fromLocal8Bit("价格"),this);
    tradeboard.price->setGeometry(begin_3, height_0, 40, height_4);
    tradeboard.price->setAlignment(Qt::AlignLeft);
    tradeboard.price->setFont(ft12);
    tradeboard.instrumentPrice_spin = new QDoubleSpinBox(this);
    tradeboard.instrumentPrice_spin->setMinimum(0);
    tradeboard.instrumentPrice_spin->setMaximum(1000000);
    tradeboard.instrumentPrice_spin->setSingleStep(1);
    tradeboard.instrumentPrice_spin->setGeometry(260, height_0, 120, height_4);
    tradeboard.instrumentPrice_spin->setFont(ft12);

    // 12
    height_0 += 30;
    tradeboard.openclose = new QLabel(QString::fromLocal8Bit("开平"),this);
    tradeboard.openclose->setGeometry(begin_1, height_0, 40, height_4);
    tradeboard.openclose->setAlignment(Qt::AlignLeft);
    tradeboard.openclose->setFont(ft12);
    tradeboard.openclose_com = new QComboBox(this);
    tradeboard.openclose_com->addItem(QString::fromLocal8Bit("开仓"));
    tradeboard.openclose_com->addItem(QString::fromLocal8Bit("平仓"));
    tradeboard.openclose_com->addItem(QString::fromLocal8Bit("平今"));
    tradeboard.openclose_com->addItem(QString::fromLocal8Bit("平昨"));
    tradeboard.openclose_com->setGeometry(60, height_0, 120, height_4);
    tradeboard.openclose_com->setFont(ft12);
    tradeboard.pricetype = new QLabel(QString::fromLocal8Bit("类型"),this);
    tradeboard.pricetype->setGeometry(begin_3, height_0, 40, height_4);
    tradeboard.pricetype->setAlignment(Qt::AlignLeft);
    tradeboard.pricetype->setFont(ft12);
    tradeboard.pricetype_com = new QComboBox(this);
    tradeboard.pricetype_com->addItem(QString::fromLocal8Bit("限价"));
    tradeboard.pricetype_com->addItem(QString::fromLocal8Bit("最新价"));
    tradeboard.pricetype_com->addItem(QString::fromLocal8Bit("对手价"));
    tradeboard.pricetype_com->addItem(QString::fromLocal8Bit("市价"));
    tradeboard.pricetype_com->setCurrentIndex(2);
    tradeboard.pricetype_com->setGeometry(260, height_0, 120, height_4);
    tradeboard.pricetype_com->setFont(ft12);

    // 13
    height_0 += 30;
    tradeboard.direction = new QLabel(QString::fromLocal8Bit("方向"),this);
    tradeboard.direction->setGeometry(begin_1, height_0, 40, height_4);
    tradeboard.direction->setAlignment(Qt::AlignLeft);
    tradeboard.direction->setFont(ft12);
    tradeboard.direction_com = new QComboBox(this);
    tradeboard.direction_com->addItem(QString::fromLocal8Bit("买多"));
    tradeboard.direction_com->addItem(QString::fromLocal8Bit("卖空"));
    tradeboard.direction_com->setGeometry(60, height_0, 120, height_4);
    tradeboard.direction_com->setFont(ft12);
    tradeboard.amount = new QLabel(QString::fromLocal8Bit("数量"),this);
    tradeboard.amount->setGeometry(begin_3, height_0, 40, height_4);
    tradeboard.amount->setAlignment(Qt::AlignLeft);
    tradeboard.amount->setFont(ft12);
    tradeboard.amount_spin = new QSpinBox(this);
    tradeboard.amount_spin->setMinimum(1);
    tradeboard.amount_spin->setMaximum(99);
    tradeboard.amount_spin->setSingleStep(1);
    tradeboard.amount_spin->setGeometry(260, height_0, 120, height_4);
    tradeboard.amount_spin->setFont(ft12);

    // 14
    height_0 += 40;
    tradeboard.xiadan = new QPushButton(this);
    tradeboard.xiadan->setStyleSheet( "QPushButton{font-family:'Microsoft YaHei';font-size:16px;background-color: rgb(255, 128, 64);}");
    tradeboard.xiadan->setGeometry(begin_1, height_0, 180, 60);
    tradeboard.xiadan->setText(QString::fromLocal8Bit("下单"));
    tradeboard.cancel = new QPushButton(this);
    tradeboard.cancel->setStyleSheet( "QPushButton{font-family:'Microsoft YaHei';font-size:16px;background-color: rgb(40, 103, 250);}");
    tradeboard.cancel->setGeometry(begin_3, height_0, 180, 60);
    tradeboard.cancel->setText(QString::fromLocal8Bit("一键全撤"));

    // 15
    height_0 += 70;
    tradeboard.set_winloss = new QLabel(QString::fromLocal8Bit("<u>下止盈止损单</u>"), this);
    tradeboard.set_winloss->setGeometry(20, height_0, 200- 20*2 , height_4);
    tradeboard.set_winloss->setAlignment(Qt::AlignCenter);
    tradeboard.set_winloss->installEventFilter(this);
    tradeboard.set_condition = new QLabel(QString::fromLocal8Bit("<u>下条件单</u>"), this);
    tradeboard.set_condition->setGeometry(220, height_0, 200- 20*2, height_4);
    tradeboard.set_condition->setAlignment(Qt::AlignCenter);
    tradeboard.set_condition->installEventFilter(this);

}

bool Tradeboard::eventFilter(QObject *target, QEvent *event)
{
    if(target  == tradeboard.instrument_info)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                QMap <QString, InstrumentInfo>& allInstruments = m_me->getInstrumentInfo();
                QString instrument = tradeboard.instrument_show->text();
                if(allInstruments.contains(instrument))
                {
                    InstrumentInfo& info = allInstruments[instrument];
                    QString information = QString::fromLocal8Bit("代码:%1,\n名称:%2,\n交易所:%3,\n保证金率:%4,\n合约乘数:%5, \n开仓手续费:%6, \n平仓手续费:%7, \n平今手续费:%8, \n最小变动单位%9,\n最后交割日%10")
                            .arg(info.symbol).arg(info.name).arg(info.exchangeId).arg(info.marginRate).arg(info.multiplier)
                            .arg(info.openCommission).arg(info.closeCommission).arg(info.closeTodayCommission).arg(info.minimumUnit).arg(info.deadline.toString("yyyy.MM.dd"));
                    int ret = QMessageBox::information(this, QString::fromLocal8Bit("合约信息"), information, QMessageBox::Ok);
                    return ret;
                }
            }
        }
    }

    if(target == tradeboard.set_winloss)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_winLoss->show();
                return true;
            }
            return false;
        }
        return false;
    }

    if(target == tradeboard.set_condition)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_condition->show();
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

bool Tradeboard::getOrderInfo(orderCommonRequest *order_info)
{
    order_info->userid = QString("ALL");     //tradeboard.account_com->currentText();   //从界面拿下单id
    order_info->instrument = tradeboard.instrument_show->text();
    order_info->quantity  = tradeboard.amount_spin->text().toInt();
    order_info->pricetype = THOST_FTDC_OPT_LimitPrice;
    order_info->type = THOST_FTDC_CIDT_Speculation;
    order_info->localType = OrderLocalType::clickOpen;
    order_info->exchangeid = m_me->getExchangeid(order_info->instrument);

    // price
    order_info->price = 0;
    if (0 == tradeboard.pricetype_com->currentIndex())  //限价
    {
        order_info->price = tradeboard.instrumentPrice_spin->text().toDouble();
    }
    else if(1 == tradeboard.pricetype_com->currentIndex())   //市价按最新成交价
    {
        order_info->price = tradeboard.lastPrice_show->text().toDouble();
    }
    else if(2 == tradeboard.pricetype_com->currentIndex())   //按对手价成交
    {
        if(0== tradeboard.direction_com->currentIndex())      //买，按卖一价买
        {
            order_info->price = tradeboard.AskPrice1_show->text().toDouble();
        }
        else if(1==tradeboard.direction_com->currentIndex()) //卖，按买一价卖
        {
            order_info->price = tradeboard.BidPrice1_show->text().toDouble();
        }
    }
    else if(3 == tradeboard.pricetype_com->currentIndex())   //市价（涨跌停）
    {
        if(0 == tradeboard.direction_com->currentIndex())      //买，按涨停价买
        {
            order_info->price = tradeboard.upperPrice_show->text().toDouble();
        }
        else if(1== tradeboard.direction_com->currentIndex()) //卖，按跌停价卖
        {
            order_info->price = tradeboard.lowerPrice_show->text().toDouble();
        }
    }

    // pricetype
//    if(3 == tradeboard.pricetype_com->currentIndex())   //市价（涨跌停）
//    {
//        order_info->pricetype = THOST_FTDC_OPT_LimitPrice;
//    }
//    else
//    {
//        order_info->pricetype = THOST_FTDC_OPT_LimitPrice;
//    }

    // direction
    if (0 == tradeboard.direction_com->currentIndex())
    {
        order_info->direction = THOST_FTDC_D_Buy;
    }
    else if (1 == tradeboard.direction_com->currentIndex())
    {
        order_info->direction = THOST_FTDC_D_Sell;
    }
    else
    {
        order_info->direction = 'a';
    }

    // offset
    if (0 == tradeboard.openclose_com->currentIndex())
    {
        order_info->offset = THOST_FTDC_OF_Open;
    }
    else if (1== tradeboard.openclose_com->currentIndex())
    {
        order_info->offset = THOST_FTDC_OF_Close;
    }
    else if (2== tradeboard.openclose_com->currentIndex())
    {
        order_info->offset = THOST_FTDC_OF_CloseToday;
    }
    else if (3== tradeboard.openclose_com->currentIndex())
    {
        order_info->offset = THOST_FTDC_OF_CloseYesterday;
    }
    else
    {
        order_info->offset = 'a';
    }

    // 判断合约为空
    if ('a' == order_info->direction || 'a' == order_info->offset
            || (0 == tradeboard.pricetype_com->currentIndex() && abs(order_info->price) < 0.00000001)
            || order_info->exchangeid == "")
    {
        qDebug()<< QString::fromLocal8Bit("下单数据异常");
        qDebug()<< QString::fromLocal8Bit("下单价格：")
                << order_info->price
                << QString::fromLocal8Bit("价格类型:")
                << order_info->pricetype
                << QString::fromLocal8Bit("下单方向:")
                << order_info->direction
                << QString::fromLocal8Bit("下单开平:")
                << order_info->offset
                << QString::fromLocal8Bit("交易所:")
                << order_info->exchangeid;

        return false;
    }
    return true;
}

void Tradeboard::addConnect()
{
    // 下单
    connect(tradeboard.xiadan, &QPushButton::clicked, [=](){
        orderCommonRequest order;
        if(getOrderInfo(&order))
        {
            QString result = m_me->meSendOrder(&order);
        }
        else
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("数据异常"),QString::fromLocal8Bit("下单数据异常，请检查后重新下单"),QMessageBox::Ok | QMessageBox::Cancel);
        }
    });

    // 撤单
    connect(tradeboard.cancel, &QPushButton::clicked,[=](){
        QMessageBox::StandardButton sb;
        QString user_id = tradeboard.account_com->currentText();
        QString content = QString::fromLocal8Bit("确认取消 %1 的所有未完成订单？\n (一次只能撤一个账户的未完成订单, ALL无效)").arg(user_id);
        sb = QMessageBox::warning(this, QString::fromLocal8Bit("取消订单"), content,QMessageBox::Ok | QMessageBox::Cancel);
        if(sb == QMessageBox::Ok)
        {
            m_me->meCancelAllorder(user_id);
        }
    });
}

