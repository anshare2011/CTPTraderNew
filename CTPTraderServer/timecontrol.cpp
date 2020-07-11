#include "timecontrol.h"
#include "ui_timecontrol.h"

TimeControl::TimeControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeControl)
{
    ui->setupUi(this);
    ui->radioButton_day->setChecked(true);
    ui->radioButton_night->setChecked(true);
}

TimeControl::~TimeControl()
{
    delete ui;
}

void TimeControl::on_pushButton_clicked()
{
    autoTimeControl time;
    if(ui->radioButton_day->isChecked())
        time.is_day = true;
    else
        time.is_day= false;
    time.day_begin = ui->begin_1->time();
    time.day_end = ui->end_1->time();
    time.night_begin = ui->begin_2->time();
    time.night_end = ui->end_2->time();

    emit sendTime(time);
    this->close();
}
