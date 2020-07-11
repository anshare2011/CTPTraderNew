#include "mypushbutton.h"

MyPushButton::MyPushButton(QWidget *parent):QPushButton(parent)
{
    this->setStyleSheet( "QPushButton{background-color: rgb( 69, 137, 148);font-family:'Microsoft YaHei';font-size:50px;color:#666666;}");

}

void MyPushButton::keyPressEvent(QKeyEvent *ev)
{
    QString text  = ev->text();
    this->setText(text);
}
