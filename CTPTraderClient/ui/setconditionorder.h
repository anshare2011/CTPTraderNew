#ifndef SETCONDITIONORDER_H
#define SETCONDITIONORDER_H

/*
 * SetConditionOrder 设置条件单
 * 1.   条件单 主要用于开仓操作
 */

#include <QWidget>

namespace Ui {
class SetConditionOrder;
}

class SetConditionOrder : public QWidget
{
    Q_OBJECT

public:
    explicit SetConditionOrder(QWidget *parent = nullptr);
    ~SetConditionOrder();

private:
    Ui::SetConditionOrder *ui;
};

#endif // SETCONDITIONORDER_H
