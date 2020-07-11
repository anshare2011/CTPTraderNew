#include "setstopwinloss.h"
#include "ui_setstopwinloss.h"

SetStopWinLoss::SetStopWinLoss(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetStopWinLoss)
{
    ui->setupUi(this);
    // this->setStyleSheet("font:11pt ;background-color: rgb( 90, 90, 90);color: white;");
    this->setWindowTitle(QString::fromLocal8Bit("设置止盈止损下单"));
}

SetStopWinLoss::~SetStopWinLoss()
{
    delete ui;
}

void SetStopWinLoss::on_pushButton_clicked()
{

}
