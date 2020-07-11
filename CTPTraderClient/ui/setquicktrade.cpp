#include "setquicktrade.h"
#include "ui_setquicktrade.h"

SetQuickTrade::SetQuickTrade(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetQuickTrade)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    this->setStyleSheet("font:11pt ;background-color: rgb( 90, 90, 90);color: white;");
    this->setWindowTitle(QString::fromLocal8Bit("设置快捷交易"));
}

SetQuickTrade::~SetQuickTrade()
{
    delete ui;
}

void SetQuickTrade::receiveKeyPressEvent(QKeyEvent *event)
{
    bool ret = (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) ||
            (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z);

    //  过滤掉空格
    if(!ret)
        return;
    // 区分大小写
    if(is_open_buy && text_open_buy == event->text())
        emit sendQuickType(QuickOpenBuy);
    else if(is_open_sell && text_open_sell == event->text())
        emit sendQuickType(QuickOpenSell);
    else if(is_offset_buy && text_offset_buy == event->text())
        emit sendQuickType(QuickOffsetBuy);
    else if(is_offset_sell && text_offset_sell == event->text())
        emit sendQuickType(QuickOffsetSell);
}

void SetQuickTrade::on_pushButton_sure_clicked()
{
    if(ui->checkBox_openbuy->isChecked())
    {
        is_open_buy = true;
        text_open_buy = ui->pushButton_openbuy->text();
    }
    else
    {
        is_open_buy = false;
    }

    if(ui->checkBox_opensell->isChecked())
    {
        is_open_sell = true;
        text_open_sell = ui->pushButton_opensell->text();
    }
    else
    {
        is_open_sell = false;
    }

    if(ui->checkBox_offsetbuy->isChecked())
    {
        is_offset_buy = true;
        text_offset_buy = ui->pushButton_offsetbuy->text();
    }
    else
    {
        is_offset_buy = false;
    }

    if(ui->checkBox_offsetsell->isChecked())
    {
        is_offset_sell = true;
        text_offset_sell = ui->pushButton_offsetsell->text();
    }
    else
    {
        is_offset_sell = false;
    }

    this->close();
}
