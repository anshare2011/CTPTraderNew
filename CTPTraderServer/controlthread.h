#ifndef CONTROLTHREAD_H
#define CONTROLTHREAD_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QDebug>
#include <QDateTime>
#include "publicstruct.h"

// 定时登录退出

class ControlThread : public QThread// public QObject, public QThread
{
	Q_OBJECT
public:
	explicit ControlThread(QObject *parent = 0);

signals:
    void mdtdIsLogin(bool);

protected:
    void run();

public:
	void stop();

public slots:
    void receiveTime(autoTimeControl time);

private:
    bool is_login = false;
    bool is_day = true;
	bool is_runing = true;
    QTime day_begin = QTime(9, 00);
    QTime day_end = QTime(15, 30);
    bool is_night= true;
    QTime night_begin = QTime(21, 00);
    QTime night_end = QTime(3, 00);

};

#endif // CONTROLTHREAD_H
