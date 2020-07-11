#include "mytoolbar.h"


MyToolBar::MyToolBar()
{
    toolButtonList.clear();
    tickToolbutton = new QToolButton();
    tickToolbutton->setText(QString::fromLocal8Bit("分时"));
    tickToolbutton->setStyleSheet("QToolButton{color: yellow;}");  //background: rgb(42, 42, 42);
    toolButtonList.append(tickToolbutton);
    //    toolbar->addWidget(tickToolbutton);

    min1Toolbutton = new QToolButton();
    min1Toolbutton->setText(QString::fromLocal8Bit("1分钟"));
    min1Toolbutton->setStyleSheet("QToolButton{color: rgb(0, 160, 230);}");
    min1Toolbutton->setCheckable(true);
    min1Toolbutton->setChecked(true);
    checked_toolbutton = min1Toolbutton;
    toolButtonList.append(min1Toolbutton);

    min3Toolbutton = new QToolButton();
    min3Toolbutton->setText(QString::fromLocal8Bit("3分钟"));
    min3Toolbutton->setStyleSheet("QToolButton{color: yellow;}");
    toolButtonList.append(min3Toolbutton);

    min5Toolbutton = new QToolButton();
    min5Toolbutton->setText(QString::fromLocal8Bit("5分钟"));
    min5Toolbutton->setStyleSheet("QToolButton{color: yellow;}");
    toolButtonList.append(min5Toolbutton);

    hour1Toolbutton = new QToolButton();
    hour1Toolbutton->setText(QString::fromLocal8Bit("1小时"));
    hour1Toolbutton->setStyleSheet("QToolButton{color: yellow;}");
    toolButtonList.append(hour1Toolbutton);

    day1Toolbutton = new QToolButton();
    day1Toolbutton->setText(QString::fromLocal8Bit("1天"));
    day1Toolbutton->setStyleSheet("QToolButton{color: yellow;}");
    toolButtonList.append(day1Toolbutton);

}

MyToolBar::~MyToolBar()
{
    delete tickToolbutton;
    delete min1Toolbutton;
    delete min3Toolbutton;
    delete min5Toolbutton;
    delete hour1Toolbutton;
    delete day1Toolbutton;
}

void MyToolBar::addToolButton(QToolBar *toolbar)
{
    for (QList<QToolButton *>::iterator item = toolButtonList.begin(); item != toolButtonList.end(); ++item)
    {
        (*item)->setCheckable(true);
        toolbar->addWidget(*item);
        connect(*item, &QToolButton::clicked, [this, item](){
            for (QList<QToolButton *>::iterator item1 = toolButtonList.begin(); item1 != toolButtonList.end(); ++item1)
            {
                if(*item == *item1){
                    (*item1)->setChecked(true);
                    (*item1)->setStyleSheet("QToolButton{color: rgb(0, 160, 230);}");
                    checked_toolbutton = (*item1);
                    QString currenrText = (*item1)->text();
                    // tell klinetab current text
//                    m_kline->setCurrentFrequency(currenrText);

                }
                else{
                    (*item1)->setChecked(false);
                    (*item1)->setStyleSheet("QToolButton{color: yellow;}");
                }
            }
        });
    }
}
