#ifndef RISKMANAGEMENT_H
#define RISKMANAGEMENT_H

/*
 * RiskManagement 管理条件单、止盈止损单
 * 1.   RiskManagement是MainEngine的成员属性，接收行情，判断条件单、止盈止损单
 * 2.   满足条件，通过信号与槽将下单信息发给 MainEngine 进行下单
 */

#include <QObject>

class RiskManagement : public QObject
{
    Q_OBJECT
public:
    explicit RiskManagement(QObject *parent = nullptr);

signals:

};

#endif // RISKMANAGEMENT_H
