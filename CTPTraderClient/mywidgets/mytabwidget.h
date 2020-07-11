#ifndef MYTABWIDGET_H
#define MYTABWIDGET_H

#include <QTabWidget>
#include <QString>
#include <QSet>
#include <QGridLayout>
#include <QDebug>
#include <QPaintEvent>
#include "mytabbar.h"
#include "mywidget.h"
//#include "../klinewidget.h"

class MyTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MyTabWidget(QTabWidget *parent = 0);
    virtual ~MyTabWidget(){}

signals:
    void sendCurrentSymbolToTradeboard(QString symbol);

public slots:
    void slot_tabBarDoubleClicked();            //响应双击弹回的槽函数

    void slot_tabDrag(int index,QPoint point);  //响应拖动动作的槽函数

    void slot_closeTab(int);

protected:
    // 画边框
    // void paintEvent(QPaintEvent *event);

public:
    QSet <QString> all_float;
    QString current_symbol = "";

private:
     MyTabBar *my_tabBar;
};

#endif // MYTABWIDGET_H
