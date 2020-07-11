#include "mytabbar.h"

MyTabBar::MyTabBar(QTabBar *parent) : QTabBar(parent),pressFlag(false)
{

}

void MyTabBar::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (event->type()==QEvent::MouseButtonDblClick)
    {
        pressFlag = false;
        emit sig_tabDrag(currentIndex(),event->pos());
        return;
    }
    if (event->button()==Qt::LeftButton)
    {
        pressFlag = true;
    }
    // 将事件传给父类处理
    // 当我们重载event()函数时, 需要调用父类的event()函数来处理我们不需要处理或是不清楚如何处理的事件.
    QTabBar::mousePressEvent(event);      // 没有这一句鼠标单击按下释放后页面会弹出
}

void MyTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton &&pressFlag )
    {
        pressFlag = false;
        // 单击tab页，换标的
        if(tabRect(currentIndex()).contains( event->pos()))
            return;
        emit sig_tabDrag(currentIndex(),event->pos());
    }
}
