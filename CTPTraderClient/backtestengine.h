#ifndef BACKTESTENGINE_H
#define BACKTESTENGINE_H

/*
 * BackTestEngine 回测引擎
 * 1.   获取行情数据，进行循环， Strategy判断是否下单，通过 SimulateEngine 进行下单
 * 2.   SimulateEngine 下单给SimulatedSettlement 进行结算
 * 3.   回测开独立线程，注意加锁
 */

#include <QObject>

class BackTestEngine : public QObject
{
    Q_OBJECT
public:
    explicit BackTestEngine(QObject *parent = nullptr);

signals:

};

#endif // BACKTESTENGINE_H
