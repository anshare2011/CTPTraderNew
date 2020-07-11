#ifndef SIMULATEENGINE_H
#define SIMULATEENGINE_H

#include <QObject>

/*
 * SimulateEngine 模拟下单引擎
 * 1.   下单给 SimulatedSettlement 进行结算
 */


class SimulateEngine : public QObject
{
    Q_OBJECT
public:
    explicit SimulateEngine(QObject *parent = nullptr);

signals:

};

#endif // SIMULATEENGINE_H
