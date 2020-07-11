#ifndef STRATEGYMANAGEMENT_H
#define STRATEGYMANAGEMENT_H

/*
 * StrategyManageMent 策略管理的界面类
 * 1.
 */

#include <QWidget>
#include "publicstruct.h"
#include "onesymbolklinedata.h"

namespace Ui {
class StrategyManageMent;
}

class StrategyManageMent : public QWidget
{
    Q_OBJECT

public:
    explicit StrategyManageMent(QWidget *parent = nullptr);
    ~StrategyManageMent();

public slots:
    // 接收行情，分发给策略
    void receiveKlineInformation(QString symbol, OneSymbolKlineData *data);

private:
    Ui::StrategyManageMent *ui;
};

#endif // STRATEGYMANAGEMENT_H
