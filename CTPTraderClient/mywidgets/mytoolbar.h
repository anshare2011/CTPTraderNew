#ifndef MYTOOLBAR_H
#define MYTOOLBAR_H

#include <QObject>
#include <QToolButton>
#include <QToolBar>
#include <QList>

class MyToolBar: public QObject
{
public:
    MyToolBar();
    ~MyToolBar();

    void addToolButton(QToolBar *toolbar);

private:
    QToolButton *tickToolbutton{nullptr};
    QToolButton *min1Toolbutton{nullptr};
    QToolButton *min3Toolbutton{nullptr};
    QToolButton *min5Toolbutton{nullptr};
    QToolButton *hour1Toolbutton{nullptr};
    QToolButton *day1Toolbutton{nullptr};

    QList<QToolButton *> toolButtonList;
    QToolButton * checked_toolbutton{nullptr};
};

#endif // MYTOOLBAR_H
