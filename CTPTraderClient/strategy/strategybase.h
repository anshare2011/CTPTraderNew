#ifndef STRATEGYBASE_H
#define STRATEGYBASE_H

#include <QObject>
#include "onesymbolklinedata.h"

/*
 * StrategyBase 策略基类
 * 1.
 */

class StrategyBase : public QObject
{
    Q_OBJECT
public:
    StrategyBase() = delete;
    explicit StrategyBase(QString strategyName, QObject *parent = nullptr);

    // 接收到合约信息
    virtual void receiveKlineInformation(QString symbol, OneSymbolKlineData *data) = 0;

    // 下单，包括开平
    virtual void sendOrder() = 0;

    // 撤单
    virtual void cancelOrder() = 0;

    // 开始停止策略
    void startStrategy();
    void stopStrategy();

signals:

private:
    QString m_strategy_name;
    bool is_start = false;               // 是否开启了策略

};

#endif // STRATEGYBASE_H
