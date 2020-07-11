#ifndef SETQUICKTRADE_H
#define SETQUICKTRADE_H

#include <QWidget>
#include <QDebug>
#include "mywidgets/mypushbutton.h"
#include "publicstruct.h"

namespace Ui {
class SetQuickTrade;
}

class SetQuickTrade : public QWidget
{
    Q_OBJECT

public:
    explicit SetQuickTrade(QWidget *parent = 0);
    ~SetQuickTrade();

    // 接收按键
    void receiveKeyPressEvent(QKeyEvent *event);

signals:
    void sendQuickType(QuickType);

private slots:
    void on_pushButton_sure_clicked();

private:
    Ui::SetQuickTrade *ui;

    bool is_open_buy = false;
    bool is_open_sell = false;
    bool is_offset_buy = false;
    bool is_offset_sell = false;

    QString text_open_buy = "";
    QString text_open_sell = "";
    QString text_offset_buy = "";
    QString text_offset_sell = "";
};

#endif // SETQUICKTRADE_H
