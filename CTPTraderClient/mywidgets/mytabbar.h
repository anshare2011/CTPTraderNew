#ifndef MYTABBAR_H
#define MYTABBAR_H

#include <QTabBar>
#include <QMouseEvent>

class MyTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit MyTabBar(QTabBar *parent = 0);
    virtual ~MyTabBar(){}

protected:
    //通过两个事件模拟出tab被拖动的动作
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);


private:
    bool pressFlag;
signals:
    void sig_tabDrag(int, QPoint);
};

#endif // MYTABBAR_H
