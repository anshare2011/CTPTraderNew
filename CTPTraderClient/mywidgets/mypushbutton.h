#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QKeyEvent>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent=0);

protected:
    void keyPressEvent(QKeyEvent *ev);

signals:

public slots:
};

#endif // MYPUSHBUTTON_H
