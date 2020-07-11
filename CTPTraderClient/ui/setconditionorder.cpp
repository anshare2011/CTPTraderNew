#include "setconditionorder.h"
#include "ui_setconditionorder.h"

SetConditionOrder::SetConditionOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetConditionOrder)
{
    ui->setupUi(this);
    // this->setStyleSheet("font:11pt ;background-color: rgb( 90, 90, 90);color: white;");
    this->setWindowTitle(QString::fromLocal8Bit("设置条件单"));
}

SetConditionOrder::~SetConditionOrder()
{
    delete ui;
}
