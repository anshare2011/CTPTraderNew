#include "kvolumegrid.h"

kVolumeGrid::kVolumeGrid()
{
    setAtomGridHeightMin(40);
    this->setMarginLeft(1);
    this->setMarginRight(100);
    this->setMarginBottom(20);
    this->smallPercent = 0.9;   // K线宽度显示90%
    this->initial();
}

void kVolumeGrid::initial()
{
    beginDay = 0;
    endDay = mydata.size() -1;
    totalDay = mydata.size();
    currentDay = totalDay /2;
}

void kVolumeGrid::fresh()
{
    endDay = mydata.size() -1;
    totalDay = mydata.size();
}

void kVolumeGrid::drawYtick()
{
    calculateIndicator();

    QPainter painter(this);
    QPen     pen;
    pen.setColor(Qt::red);
    painter.setPen(pen);
    double ystep = maxVolume / getHGridNum() ;
    QString str;
    for( int i=0;i<= getHGridNum();++i)
    {
        if(i == getHGridNum()){
            str.sprintf("%d", (int)(i*ystep) );
            painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10,
                                      getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight() + 10),
                              str);
        }
        else if(i == 0)
        {
            str.sprintf("%d", (int)(i*ystep) );
            painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10,
                                      getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight() -10),
                              str);
        }
        else{
            str.sprintf("%d", (int)(i*ystep) );
            painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10,
                                      getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight()),
                              str);}
    }
}

void kVolumeGrid::drawVolume()
{
    QPainter painter(this);
    QPen     pen;
    pen.setCapStyle(Qt::FlatCap);

    double yscale = getGridHeight() / maxVolume;

    double xstep;
    if(totalDay == beginDay)
        xstep = 16;
    else
    {
        xstep = floor((getGridWidth() - 100) / (totalDay - beginDay));   //预留100显示信息
        if( xstep < 2)
            xstep = 2;
        // 此处要改变 beginDay的值，如果beginDay的值导致xstep<2加大beginDay，否则不变
        else if(xstep >16)
            xstep = 16;
    }

    //为了各个k线之间不贴在一起，设置一个间隔
    lineWidth = smallPercent * xstep;

    //画线连接的两个点
    QPoint p1;
    QPoint p2;

    QPoint p3;
    QPoint p4;

    for( int i= beginDay ;i<totalDay; ++i)
    {
        int temp = mydata[i].volume;
        //阴线
        if( mydata[i].open > mydata[i].close )
        {
            pen.setColor(QColor(85,252,252));
            pen.setWidth(lineWidth);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight()  - getMarginBottom());
            painter.drawLine(p1,p2);

        }
        //阳线
        else
        {
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(i - beginDay +0.1) );
            p1.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay +0.9) +1);
            p2.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p3.setX( getMarginLeft() + xstep *(i - beginDay +0.1) );
            p3.setY( getWidgetHeight()  - getMarginBottom() -1);      //-1让下面的线显示出来
            p4.setX( getMarginLeft() + xstep *(i - beginDay +0.9) +1);
            p4.setY( getWidgetHeight()  - getMarginBottom() -1);

            painter.drawLine(p1,p2);
            painter.drawLine(p1,p3);
            painter.drawLine(p2,p4);
            painter.drawLine(p3,p4);

        }

        if(0 == mydata[i].currentMinute %30)
        {
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep );
            p1.setY( getWidgetHeight() - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - getMarginBottom() +4);
            painter.drawLine(p1,p2);

            double xPos =  getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep;
            double yPos =  getWidgetHeight()  - getMarginBottom();
            painter.drawText(xPos - 30, yPos +15, mydata[i].time);
        }
    }

    if(is_unfinished)
    {
        int temp = unfinishedBar.volume;
        //阴线
        if( unfinishedBar.open > unfinishedBar.close )
        {
            pen.setColor(QColor(85,252,252));
            pen.setWidth(lineWidth);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight()  - getMarginBottom());
            painter.drawLine(p1,p2);

        }
        //阳线
        else
        {
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.1) );
            p1.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.9) +1);
            p2.setY( getWidgetHeight() - (temp ) *yscale - getMarginBottom());
            p3.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.1) );
            p3.setY( getWidgetHeight()  - getMarginBottom() -1);      //-1让下面的线显示出来
            p4.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.9) +1);
            p4.setY( getWidgetHeight()  - getMarginBottom() -1);

            painter.drawLine(p1,p2);
            painter.drawLine(p1,p3);
            painter.drawLine(p2,p4);
            painter.drawLine(p3,p4);

        }

        if(0 == unfinishedBar.currentMinute %30)
        {
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep );
            p1.setY( getWidgetHeight() - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - getMarginBottom() +4);
            painter.drawLine(p1,p2);

            double xPos =  getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep;
            double yPos =  getWidgetHeight()  - getMarginBottom();
            painter.drawText(xPos - 30, yPos +15, unfinishedBar.time);
        }
    }
}

void kVolumeGrid::calculateIndicator()
{
    maxVolume = 1;
    for(int i=beginDay ;i<totalDay; ++i)
    {
        if( mydata[i].volume > maxVolume)
        {
            maxVolume = mydata[i].volume;
        }
    }

    if(is_unfinished)
    {
        if( unfinishedBar.volume > maxVolume )
            maxVolume = unfinishedBar.volume;
    }
}

void kVolumeGrid::drawAverageLine(int day)
{

}

void kVolumeGrid::calvolumeAverage(int size1)
{

}

void kVolumeGrid::calvolumeAverage5()
{

}

void kVolumeGrid::calvolumeAverage10()
{

}

void kVolumeGrid::setBegin(int begin)
{
    this->beginDay = begin;
}

void kVolumeGrid::setMyData(QVector<BarData>& data, BarData data1)
{
    is_unfinished = true;
    mydata = data;
    unfinishedBar = data1;
    if(!isInit)
    {
        isInit = true;
        initial();
    }
    update();
}

void kVolumeGrid::paintEvent(QPaintEvent *event)
{
    AutoGrid::paintEvent(event);

    fresh();

    //画y轴坐标轴
    drawYtick();

    drawVolume();
}
