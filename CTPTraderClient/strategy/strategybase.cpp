#include "strategybase.h"

StrategyBase::StrategyBase(QString strategyName, QObject *parent) : QObject(parent)
{
    this->m_strategy_name = strategyName;
}

void StrategyBase::startStrategy()
{
    this->is_start = true;
}

void StrategyBase::stopStrategy()
{
    this->is_start = false;
}
