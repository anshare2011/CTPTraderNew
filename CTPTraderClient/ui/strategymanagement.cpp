#include "strategymanagement.h"
#include "ui_strategymanagement.h"

StrategyManageMent::StrategyManageMent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StrategyManageMent)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("策略管理"));
}

StrategyManageMent::~StrategyManageMent()
{
    delete ui;
}

void StrategyManageMent::receiveKlineInformation(QString symbol, OneSymbolKlineData *data)
{

}
