#include "controlthread.h"

ControlThread::ControlThread(QObject *parent)
{
	is_runing = true;
}

void ControlThread::run()
{
	while (is_runing){

		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_week = current_date_time.toString("ddd");
//		if (current_week == QString::fromLocal8Bit("周日") || current_week == QString::fromLocal8Bit("周六"))
//			return;

        QTime time_now = QTime::currentTime();
        int hour = time_now.hour();
        int minute = time_now.minute();
        int curent_minute = hour *60 + minute;

        // 8:55-9:00 登录, 15:25-15:30 退出, 20:55-21:00 登录, 02:55-03:00 退出,
        if(is_day)
        {
            if(curent_minute < day_begin.hour()*60 +day_begin.minute() &&
                    curent_minute > day_begin.hour()*60 +day_begin.minute() - 5)
            {
                is_login = true;
                emit mdtdIsLogin(true);
                qDebug()<< "ControlThread::run"<< QString::fromLocal8Bit("白盘连接登录");
            }
			else if (curent_minute < day_begin.hour() * 60 + day_begin.minute()-5 &&
				curent_minute > day_begin.hour() * 60 + day_begin.minute() - 10)
			{
				is_login = false;
				emit mdtdIsLogin(false);
				qDebug() << "ControlThread::run" << QString::fromLocal8Bit("白盘连接登录前退出登录");
			}
            else if(curent_minute < day_end.hour()*60 +day_end.minute() &&
                    curent_minute > day_end.hour()*60 +day_end.minute() - 5)
            {
                is_login = false;
                emit mdtdIsLogin(false);
                qDebug()<< "ControlThread::run"<< QString::fromLocal8Bit("白盘退出登录");
            }
        }

        if(is_night){
            if(curent_minute < night_begin.hour()*60 +night_begin.minute() &&
                    curent_minute > night_begin.hour()*60 +night_begin.minute() - 5)
            {
                is_login = true;
                emit mdtdIsLogin(true);
                qDebug()<< "ControlThread::run"<< QString::fromLocal8Bit("夜盘连接登录");
            }
			else if (curent_minute < night_begin.hour() * 60 + night_begin.minute()-5 &&
				curent_minute > night_begin.hour() * 60 + night_begin.minute() - 10)
			{
				is_login = false;
				emit mdtdIsLogin(false);
				qDebug() << "ControlThread::run" << QString::fromLocal8Bit("夜盘连接登录前退出");
			}
            //        else if(curent_minute < (4 * 60 + 10) && curent_minute > (4 * 60))
            else if(curent_minute < night_end.hour()*60 +night_end.minute() &&
                    curent_minute > night_end.hour()*60 +night_end.minute() - 5)
            {
                is_login = false;
                emit mdtdIsLogin(false);
                qDebug()<< "ControlThread::run"<< QString::fromLocal8Bit("夜盘退出登录");
            }
        }
        QThread::sleep(60);
    }
}

void ControlThread::stop()
{
	is_runing = false;
	this->terminate();
	this->wait();
}

void ControlThread::receiveTime(autoTimeControl time)
{
    is_day = time.is_day;
    day_begin = time.day_begin;
    day_end = time.day_end;

    is_night= time.is_night;
    night_begin = time.night_begin;
    night_end = time.night_end;
}
