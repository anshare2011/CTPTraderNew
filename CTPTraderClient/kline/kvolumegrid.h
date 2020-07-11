#ifndef KVOLUMEGRID_H
#define KVOLUMEGRID_H

#include <QObject>
#include <QPaintEvent>
#include "autogrid.h"
#include "../publicstruct.h"

class kVolumeGrid : public AutoGrid
{
public:
    kVolumeGrid();
    void initial();
    void fresh();
    void drawYtick();
    void drawVolume();
    void calculateIndicator();

    void drawAverageLine(int day);
    void calvolumeAverage(int size1);
    void calvolumeAverage5();
    void calvolumeAverage10();
    void setBegin(int);

    void setMyData(QVector<BarData>& data, BarData data1);

protected:
    void virtual paintEvent(QPaintEvent* event);

private:
    bool is_unfinished = false;
    BarData unfinishedBar;
    QVector<BarData> mydata;
    // 成交量K线显示比例
    double smallPercent;

    //画成交量线的起始日期和终止日期
    int beginDay;
    int endDay;
    int totalDay;
    int currentDay;

    //当前要画的成交量线中的最大成交量
    double maxVolume = 1;

    //鼠标位置
    QPoint mousePoint;

    //线宽
    int lineWidth;

    bool isInit = false;
};

#endif // KVOLUMEGRID_H
