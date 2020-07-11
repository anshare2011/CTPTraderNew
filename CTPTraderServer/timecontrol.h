#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <QWidget>
#include <QDebug>
#include "publicstruct.h"

namespace Ui {
class TimeControl;
}

// 修改 ControlThread 的自动登录 退出时间


class TimeControl : public QWidget
{
    Q_OBJECT

public:
    explicit TimeControl(QWidget *parent = 0);
    ~TimeControl();

signals:
    void sendTime(autoTimeControl time);

private slots:
    void on_pushButton_clicked();

private:
    Ui::TimeControl *ui;
};

#endif // TIMECONTROL_H
