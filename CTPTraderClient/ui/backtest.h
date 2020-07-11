#ifndef BACKTEST_H
#define BACKTEST_H

#include <QWidget>

namespace Ui {
class BackTest;
}

class BackTest : public QWidget
{
    Q_OBJECT

public:
    explicit BackTest(QWidget *parent = nullptr);
    ~BackTest();

private:
    Ui::BackTest *ui;
};

#endif // BACKTEST_H
