#ifndef SETSTOPWINLOSS_H
#define SETSTOPWINLOSS_H

/*
 * SetStopWinLoss 设置止盈止损
 * 1.   止盈止损止盈针对持仓表里的持仓进行平仓操作
 */

#include <QWidget>

namespace Ui {
class SetStopWinLoss;
}

class SetStopWinLoss : public QWidget
{
    Q_OBJECT

public:
    explicit SetStopWinLoss(QWidget *parent = 0);
    ~SetStopWinLoss();

signals:

    void sendStopWinPoints(double, QChar);

    void sendStopLossPoints(double, QChar);

private slots:
    void on_pushButton_clicked();

private:
    Ui::SetStopWinLoss *ui;
};

#endif // SETSTOPWINLOSS_H
