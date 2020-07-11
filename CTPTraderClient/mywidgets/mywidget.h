#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QCloseEvent>

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

protected:
    bool event(QEvent *);
    // 重写关闭事件，禁止关闭
    void closeEvent(QCloseEvent *event);

signals:
    void sig_doubleClickedTitleBar();  //被双击时发射的信号

public slots:
};

#endif // MYWIDGET_H
