#include "sendorder.h"
#include "ui_sendorder.h"

SendOrder::SendOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendOrder)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("下单板"));
    this->setStyleSheet("font:12pt '宋体';");


    ui->buy->setChecked(true);
    ui->open->setChecked(true);
    ui->priceLimit->setChecked(true);
    ui->Speculation->setChecked(true);

    // 价格设置
    ui->price->setMinimum(0);
    ui->price->setMaximum(1000000);
    ui->price->setSingleStep(1);

    // 数量设置
    ui->quantity->setMinimum(1);
    ui->quantity->setMaximum(99);
    ui->quantity->setSingleStep(1);
}

SendOrder::~SendOrder()
{
    delete ui;
}

void SendOrder::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    if(ui->instrument->text() == quote->symbol)
    {
        if(ui->priceMarket->isChecked())
        {
            if(ui->buy->isChecked())
                ui->price->setValue(quote->upperLimit);
            else
                ui->price->setValue(quote->lowerLimit);
        }
        else if(ui->priceRival->isChecked())
        {
            if(ui->buy->isChecked())
                ui->price->setValue(quote->askPrice1);
            else
                ui->price->setValue(quote->bidPrice1);
        }
        else if(ui->priceNew->isChecked())
            ui->price->setValue(quote->lastPrice);
    }
}

void SendOrder::on_pushButton_clicked()
{
    orderCommonRequest order;
    order.userid = ui->account->text();
    order.instrument = ui->instrument->text();

    if(ui->buy->isChecked())
        order.direction = THOST_FTDC_D_Buy;
    else
        order.direction = THOST_FTDC_D_Sell;

    if(ui->open->isChecked())
        order.offset = THOST_FTDC_OF_Open;
    else if(ui->close->isChecked())
        order.offset = THOST_FTDC_OF_Close;
    else if(ui->closeToday->isChecked())
        order.offset = THOST_FTDC_OF_CloseToday;

    order.pricetype = THOST_FTDC_OPT_LimitPrice;        // 统一按照限价下单
    order.price = ui->price->value();
    order.quantity = ui->quantity->value();
    order.exchangeid = m_me->getExchangeid(order.instrument);
    order.type = THOST_FTDC_CIDT_Speculation;
    order.localType = OrderLocalType::clickOpen;

    m_me->meSendOrder(&order);
}
