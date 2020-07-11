#ifndef SIMULATEDSETTLEMENT_H
#define SIMULATEDSETTLEMENT_H

#include <QObject>

/*
 * 模拟结算类
 * 1.   接收模拟账户注册
 * 2.   接收模拟账户和订单，进行结算
 */

class SimulatedSettlement : public QObject
{
    Q_OBJECT
public:
    explicit SimulatedSettlement(QObject *parent = nullptr);

signals:

};

#endif // SIMULATEDSETTLEMENT_H
