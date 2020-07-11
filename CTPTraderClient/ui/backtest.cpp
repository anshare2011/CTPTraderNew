#include "backtest.h"
#include "ui_backtest.h"

BackTest::BackTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackTest)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("回测管理"));
}

BackTest::~BackTest()
{
    delete ui;
}
