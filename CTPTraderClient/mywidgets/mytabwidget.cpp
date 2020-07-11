#include "mytabwidget.h"

MyTabWidget::MyTabWidget(QTabWidget *parent) : QTabWidget(parent)
{
    my_tabBar = new MyTabBar();
    this->setTabBar(my_tabBar);
    this->setMovable(true);
    connect(my_tabBar, SIGNAL(sig_tabDrag(int,QPoint)), this, SLOT(slot_tabDrag(int,QPoint)));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_closeTab(int)));

    connect(this, &MyTabWidget::currentChanged, [=](int index){
        this->setCurrentIndex(index);
        int current_index = my_tabBar->currentIndex();
        if(-1 == current_index)
        {
            current_symbol = "";
        }
        else
        {
            current_symbol = this->tabText(current_index);
        }
        // 通知tradeboard 标的已经变化
        emit sendCurrentSymbolToTradeboard(current_symbol);
    });
}

void MyTabWidget::slot_tabBarDoubleClicked()
{
    MyWidget *temp_widget = qobject_cast<MyWidget*>(sender());
    qDebug()<< "MyTabWidget::slot_tabBarDoubleClicked"<< temp_widget;
    if(temp_widget == NULL)
        return;

    QObjectList object_list = temp_widget->children();
    QWidget *compare_widget = NULL;
    qDebug()<< "MyTabWidget::slot_tabBarDoubleClicked"<< object_list.count();
    for(int i = 0;i<object_list.count();++i)
    {
        if(object_list[i]->inherits("KlineWidget"))
        {
            compare_widget = qobject_cast<QWidget*>(object_list[i]);
            break;    // ???????????
        }
        if(object_list[i]->inherits("QTableWidget"))
        {
            compare_widget = qobject_cast<QWidget*>(object_list[i]);
            break;
        }
        if(object_list[i]->inherits("QWidget"))
        {
            compare_widget = qobject_cast<QWidget*>(object_list[i]);
            break;
        }
    }
    if(compare_widget == NULL)
        return;

    compare_widget->setParent(this);
    this->addTab(compare_widget, temp_widget->windowTitle());

    //减少悬浮状态
    all_float.remove(temp_widget->windowTitle());

    // delete temp_widget;
    temp_widget->hide();
}

void MyTabWidget::slot_tabDrag(int index, QPoint point)
{
    MyWidget *temp_widget = new MyWidget();
    QWidget *draged = this->widget(index);
    QString tab_text = this->tabText(index);
    this->removeTab(index);

    connect(temp_widget,SIGNAL(sig_doubleClickedTitleBar()),this,SLOT(slot_tabBarDoubleClicked()));

    QGridLayout *layout = new QGridLayout;
//    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(draged);
    temp_widget->setLayout(layout);
    temp_widget->resize(1200, 600);
//    temp_widget->move(point+pos()+tabWidget->pos());
    temp_widget->setWindowTitle(tab_text);
    temp_widget->show();
    draged->show();

    //增加悬浮状态
    all_float.insert(tab_text);
}

void MyTabWidget::slot_closeTab(int index)
{
    qDebug()<< "MyTabWidget::slot_closeTab";
//    QWidget *draged = this->widget(index);
    this->removeTab(index);      // the page widget itself is noe deleted
    //    delete draged;
}

//void MyTabWidget::paintEvent(QPaintEvent *event)
//{

//}
