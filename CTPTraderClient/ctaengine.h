#ifndef CTAENGINE_H
#define CTAENGINE_H

/*
 * CtaEngine 策略引擎
 * 1.   接收行情，转给StrategyBase，判断下单
 * 2.   通过me进行下单
 */

#include <QObject>
#include "mainengine.h"
#include "strategy/strategybase.h"

class CtaEngine : public QObject
{
    Q_OBJECT
public:
    explicit CtaEngine(MainEngine *me,StrategyBase *sb);

signals:

private:
    MainEngine* m_me{ nullptr };
    StrategyBase* m_sb{ nullptr };

};

#endif // CTAENGINE_H
