#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("background-color: rgb( 42, 42, 42);color: white;");
}

MyWidget::~MyWidget()
{

}

bool MyWidget::event(QEvent * event)
{
    if (event->type() == QEvent::NonClientAreaMouseButtonDblClick)  //双击widget的标栏
    {
        emit sig_doubleClickedTitleBar();
        return true;
    }
    return QWidget::event(event);
}

void MyWidget::closeEvent(QCloseEvent *event)
{
    emit sig_doubleClickedTitleBar();
    event->ignore();
}
