#include "klinegrid.h"

KLineGrid::KLineGrid(QWidget *parent) : AutoGrid(parent)
{
    //开启鼠标追踪
    this->setMouseTracking(true);
    //    this->setMarginTop(20);
    this->setMarginLeft(1);
    this->setMarginRight(100);
    this->smallPercent = 0.9;   // K线宽度显示90%
    this->initial();
}

KLineGrid::~KLineGrid()
{

}

void KLineGrid::initial()
{
    beginDay = 0;
    endDay = mydata.size() -1;
    totalDay = mydata.size();
    currentDay = endDay / 2;
}

void KLineGrid::fresh()
{
    endDay = mydata.size() -1;
    totalDay = mydata.size();
}

void KLineGrid::drawLine()
{
    this->fresh();
    this->calAverageLine5();
    this->calAverageLine10();
    this->calAverageLine20();
    this->calAverageLine60();

    //计算y轴指标
    this->calculateIndicator();
    //显示y轴价格
    this->drawYtick();
    //画k线
    this->drawKline();
    //画5日均线
    this->drawAverageLine(5);
    //画10日均线
    this->drawAverageLine(10);
    //画20日均线
    this->drawAverageLine(20);
    //画60日均线
    this->drawAverageLine(60);
    //画十字线
    if( !isKeyDown && isCrossShow)
    {
        this->drawCross2();
    }
    this->drawTips2();
}

void KLineGrid::calculateIndicator()
{
    highestBid = 0;
    lowestBid = 1000000;
    maxVolume = 0;

    for( int i= beginDay; i<totalDay; ++i)
    {
        if( mydata[i].high > highestBid )
            highestBid = mydata[i].high;
        if( mydata[i].low < lowestBid )
            lowestBid =mydata[i].low;
    }
    if(is_unfinished)
    {
        if( unfinishedBar.high > highestBid )
            highestBid = unfinishedBar.high;
        if( unfinishedBar.low < lowestBid )
            lowestBid = unfinishedBar.low;
    }

}

void KLineGrid::drawYtick()
{
    QPainter painter(this);
    QPen     pen;
    pen.setColor(Qt::red);
    painter.setPen(pen);

    double ystep = (highestBid - lowestBid)/ getHGridNum();      //  最高价- 最低价 / Y轴列数
    QString str;

    if(0 == getHGridNum())
    {
        str.sprintf("%.2f",lowestBid);
        painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10, getWidgetHeight() - getMarginBottom() -10),
                          str);
        str.sprintf("%.2f",highestBid);
        painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10, getMarginTop() +10),
                          str);
        return;
    }

    for( int i=0;i<=getHGridNum();++i)
    {
        if(i ==0){
            str.sprintf("%.2f",lowestBid+ i*ystep);
            painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10,getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight() -10),
                              str);
        }
        else{
            str.sprintf("%.2f",lowestBid+ i*ystep);
            painter.drawText( QPoint( getWidgetWidth() - getMarginRight() + 10, getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight() +10),
                              str);
        }
    }
}

void KLineGrid::drawKline()
{
    //y轴缩放
    yscale = getGridHeight() / (highestBid -lowestBid );

    //x轴步长
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

    //画笔的线宽，为了各个k线之间不贴在一起，设置一个间隔
    lineWidth = smallPercent * xstep;


    // QPainter的绘图体系是以左上角为(0,0)坐标
    QPainter painter(this);
    QPen     pen;
    pen.setCapStyle(Qt::FlatCap);    //方线末端，不覆盖线的终点。
//    pen.setColor(Qt::red);
    painter.setPen(pen);

    //画线连接的两个点
    QPoint p1;
    QPoint p2;

    QPoint p3;
    QPoint p4;

    for( int i= beginDay ;i<totalDay; ++i)
    {
        // 画阴线
        if( mydata[i].open - mydata[i].close > 0.00000001 )
        {
            //画开盘与收盘之间的粗实线
            pen.setColor(QColor(85,252,252));
            pen.setWidth(lineWidth);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (mydata[i].open - lowestBid) *yscale - getMarginBottom());// -  1*xstep);
            p2.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - (mydata[i].close - lowestBid) *yscale - getMarginBottom());// - 0.4*xstep);   //  0.4*xstep代表线宽的一半
            painter.drawLine(p1,p2);


            //画最高价与最低价之间的细线
            pen.setWidth(1);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (mydata[i].high - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - (mydata[i].low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);


        }
        else if(mydata[i].close - mydata[i].open >0.00000001 )
        {
            //像同花顺一样阳线画成空心的
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);


            p1.setX( getMarginLeft() + xstep *(i - beginDay +0.1)  );
            p1.setY( getWidgetHeight() - (mydata[i].open - lowestBid) *yscale - getMarginBottom());

            p2.setX( getMarginLeft() + xstep *(i - beginDay +0.9) +1);
            p2.setY( getWidgetHeight() - (mydata[i].open - lowestBid) *yscale - getMarginBottom());


            p3.setX( getMarginLeft() + xstep *(i - beginDay +0.1) );
            p3.setY( getWidgetHeight() - (mydata[i].close - lowestBid) *yscale - getMarginBottom());

            p4.setX( getMarginLeft() + xstep *(i - beginDay +0.9)  +1);
            p4.setY( getWidgetHeight() - (mydata[i].close - lowestBid) *yscale - getMarginBottom());

            painter.drawLine(p1,p2);
            painter.drawLine(p1,p3);
            painter.drawLine(p2,p4);
            painter.drawLine(p3,p4);


            //画最高价与最低价之间的细线
            pen.setWidth(1);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(i - beginDay)  + 0.5*xstep);
            p1.setY( getWidgetHeight() - (mydata[i].high - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay)  + 0.5*xstep);
            p2.setY( getWidgetHeight() - (mydata[i].close - lowestBid) *yscale - getMarginBottom());
            p3.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p3.setY( getWidgetHeight() - (mydata[i].open - lowestBid) *yscale - getMarginBottom());
            p4.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p4.setY(getWidgetHeight() - (mydata[i].low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);
            painter.drawLine(p3,p4);

        }
        else
        {
            pen.setColor(Qt::white);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(i - beginDay)  + 0.1*xstep);
            p1.setY( getWidgetHeight() - (mydata[i].open - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(i - beginDay)  + 0.9*xstep +1);
            p2.setY( getWidgetHeight() - (mydata[i].close - lowestBid) *yscale - getMarginBottom());
            p3.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p3.setY( getWidgetHeight() - (mydata[i].high - lowestBid) *yscale - getMarginBottom());
            p4.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            p4.setY(getWidgetHeight() - (mydata[i].low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);
            painter.drawLine(p3,p4);
        }

    }

    if(is_unfinished)
    {
        // 画阴线
        if(unfinishedBar.open - unfinishedBar.close > 0.00000001 )
        {
            //画开盘与收盘之间的粗实线
            pen.setColor(QColor(85,252,252));
            pen.setWidth(lineWidth);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (unfinishedBar.open - lowestBid) *yscale - getMarginBottom());// -  1*xstep);
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - (unfinishedBar.close - lowestBid) *yscale - getMarginBottom());// - 0.4*xstep);   //  0.4*xstep代表线宽的一半
            painter.drawLine(p1,p2);


            //画最高价与最低价之间的细线
            pen.setWidth(1);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p1.setY( getWidgetHeight() - (unfinishedBar.high - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p2.setY( getWidgetHeight() - (unfinishedBar.low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);


        }
        else if(unfinishedBar.close - unfinishedBar.open >0.00000001 )
        {
            //像同花顺一样阳线画成空心的
            pen.setColor(Qt::red);
            pen.setWidth(1);
            painter.setPen(pen);


            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.1)  );
            p1.setY( getWidgetHeight() - (unfinishedBar.open - lowestBid) *yscale - getMarginBottom());

            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.9) +1);
            p2.setY( getWidgetHeight() - (unfinishedBar.open - lowestBid) *yscale - getMarginBottom());


            p3.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.1) );
            p3.setY( getWidgetHeight() - (unfinishedBar.close - lowestBid) *yscale - getMarginBottom());

            p4.setX( getMarginLeft() + xstep *(totalDay - beginDay +0.9) +1);
            p4.setY( getWidgetHeight() - (unfinishedBar.close - lowestBid) *yscale - getMarginBottom());

            painter.drawLine(p1,p2);
            painter.drawLine(p1,p3);
            painter.drawLine(p2,p4);
            painter.drawLine(p3,p4);


            //画最高价与最低价之间的细线
            pen.setWidth(1);
            painter.setPen(pen);
            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay)  + 0.5*xstep);
            p1.setY( getWidgetHeight() - (unfinishedBar.high - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay)  + 0.5*xstep);
            p2.setY( getWidgetHeight() - (unfinishedBar.close - lowestBid) *yscale - getMarginBottom());
            p3.setX(getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p3.setY( getWidgetHeight() - (unfinishedBar.open - lowestBid) *yscale - getMarginBottom());
            p4.setX(getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p4.setY(getWidgetHeight() - (unfinishedBar.low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);
            painter.drawLine(p3,p4);

        }
        else
        {
            pen.setColor(Qt::white);
            pen.setWidth(1);
            painter.setPen(pen);

            p1.setX( getMarginLeft() + xstep *(totalDay - beginDay)  + 0.1*xstep);
            p1.setY( getWidgetHeight() - (unfinishedBar.open - lowestBid) *yscale - getMarginBottom());
            p2.setX( getMarginLeft() + xstep *(totalDay - beginDay)  + 0.9*xstep +1);
            p2.setY( getWidgetHeight() - (unfinishedBar.close - lowestBid) *yscale - getMarginBottom());
            p3.setX(getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p3.setY( getWidgetHeight() - (unfinishedBar.high - lowestBid) *yscale - getMarginBottom());
            p4.setX(getMarginLeft() + xstep *(totalDay - beginDay) + 0.5*xstep);
            p4.setY(getWidgetHeight() - (unfinishedBar.low - lowestBid) *yscale - getMarginBottom());
            painter.drawLine(p1,p2);
            painter.drawLine(p3,p4);
        }
    }



}

void KLineGrid::setMyData(QVector<BarData>& data, BarData data1)
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

void KLineGrid::calAverageLine5()
{
    // 待优化
    for(int i=0; i<mydata.size(); ++i)
    {
        mydata[i].averageLine5 = 0;
    }

    if(mydata.size() >4)
    {
        for( int i=4;i<mydata.size();++i)
        {
            double sum = 0;
            for(int j=i-4;j<=i;++j)
            {
                sum += mydata[j].close;
            }
            mydata[i].averageLine5 = sum /5;
        }
    }
}

void KLineGrid::calAverageLine10()
{
    for(int i=0; i<mydata.size(); ++i)
    {
        mydata[i].averageLine10 = 0;
    }
    if(mydata.size() >9)
    {
        for( int i=9;i<mydata.size();++i)
        {
            double sum = 0;
            for(int j=i-9;j<=i;++j)
            {
                sum += mydata[j].close;
            }
            mydata[i].averageLine10 = sum /10;
        }
    }
}

void KLineGrid::calAverageLine20()
{
    for(int i=0; i<mydata.size(); ++i)
    {
        mydata[i].averageLine20 = 0;
    }
    if(mydata.size() >19)
    {
        for( int i=19;i<mydata.size();++i)
        {
            double sum = 0;
            for(int j=i-19;j<=i;++j)
            {
                sum += mydata[j].close;
            }
            mydata[i].averageLine20 = sum /20;
        }
    }
}

void KLineGrid::calAverageLine60()
{
    for(int i=0; i<mydata.size(); ++i)
    {
        mydata[i].averageLine60 = 0;
    }
    if(mydata.size() >59)
    {
        for( int i=59;i<mydata.size();++i)
        {
            double sum = 0;
            for(int j=i-59;j<=i;++j)
            {
                sum += mydata[j].close;
            }
            mydata[i].averageLine60 = sum /60;
        }
    }
}

void KLineGrid::drawCross2()
{
    drawMouseMoveCrossHorLine();
    drawMouseMoveCrossVerLine();

    // 展示K线详细信息
    drawDetails();
    //    drawTips2();
}

void KLineGrid::drawMouseMoveCrossVerLine()
{
    if(mousePoint.x() < getMarginLeft() || mousePoint.x() > getWidgetWidth() - getMarginRight())
        return;

    if(mousePoint.y() < getMarginTop() || mousePoint.y() > getWidgetHeight() - getMarginBottom())
        return;

    QPainter painter(this);
    QPen     pen;
    pen.setColor(QColor("#FFFFFF"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(mousePoint.x(),getMarginTop(),
                     mousePoint.x(),getWidgetHeight() - getMarginBottom());
}

void KLineGrid::drawMouseMoveCrossHorLine()
{
    if(mousePoint.x() < getMarginLeft() || mousePoint.x() > getWidgetWidth() - getMarginRight())
        return;

    if(mousePoint.y() < getMarginTop() || mousePoint.y() > getWidgetHeight() - getMarginBottom())
        return;

    QPainter painter(this);
    QPen     pen;
    pen.setColor(QColor("#FFFFFF"));
    pen.setWidth(1);
    painter.setPen(pen);

    painter.drawLine(getMarginLeft(),mousePoint.y(),
                     getWidgetWidth()-getMarginRight(),mousePoint.y());
}

void KLineGrid::drawTips2()
{
    if(mousePoint.x() < getMarginLeft() || mousePoint.x() > getWidgetWidth() - getMarginRight())
        return;

    if(mousePoint.y() < getMarginTop()  || mousePoint.y() > getWidgetHeight() - getMarginBottom())
        return;

    QPainter painter(this);
    QPen     pen;
    QBrush brush(QColor("#FFFFFF"));
    painter.setBrush(brush);
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);

    double yval =  highestBid - ( mousePoint.y() - getMarginTop() ) / yscale;
    double yPos = mousePoint.y();

    int iTipsWidth = this->getMarginRight() -2 > 80 ? 80: this->getMarginRight() -2;
    int iTipsHeight = 20;

    QString str;

    QRect rect( getWidgetWidth() - getMarginRight() +1,
                yPos - iTipsHeight/2,iTipsWidth,iTipsHeight);
    painter.drawRect(rect);


    QRect rectText( getWidgetWidth() - getMarginRight() + 10,
                    yPos - iTipsHeight/4,iTipsWidth,iTipsHeight);
    painter.drawText(rectText, Qt::AlignLeft, str.sprintf("%.2f",yval));
}

void KLineGrid::drawDetails()
{
    if(mousePoint.x() < getMarginLeft() || mousePoint.x() > getWidgetWidth() - getMarginRight())
        return;

    if(mousePoint.y() < getMarginTop() || mousePoint.y() > getWidgetHeight() - getMarginBottom())
        return;

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

    if(mousePoint.x() - getMarginLeft() <0)
        return;

    QPainter painter(this);
    QPen     pen;
    QBrush brush(QColor(214, 200, 75));
    painter.setBrush(brush);
    pen.setColor(QColor("#FFFFFF"));
    pen.setWidth(1);
    painter.setPen(pen);

    currentDay = ( mousePoint.x() - getMarginLeft() ) / xstep + beginDay;
    if(currentDay > totalDay)
        return;
    else if(currentDay == totalDay)
    {
        if(is_unfinished)
        {
            QRect rectText( getGridWidth() - 80, getMarginTop()+1, 80, 300);
            painter.drawRect(rectText);
            QString strMessage = QString::fromLocal8Bit("时间\n%1\n开盘\n%2\n最高\n%3\n最低\n%4\n收盘\n%5\n成交量\n%6\n涨跌\n%7\n涨跌幅(%)\n%8\n")
                    .arg(unfinishedBar.time).arg(unfinishedBar.open).arg(unfinishedBar.high).arg(unfinishedBar.low)
                    .arg(unfinishedBar.close).arg(unfinishedBar.volume).arg(unfinishedBar.close - unfinishedBar.open)
                    .arg(QString::number(100.0* (unfinishedBar.close - unfinishedBar.open) / unfinishedBar.open  , 'f',2));

            painter.drawText(rectText,Qt::AlignCenter, strMessage);
        }
    }
    else
    {
        QRect rectText( getGridWidth() - 80, getMarginTop()+1, 80, 300);
        painter.drawRect(rectText);

        QString strMessage = QString::fromLocal8Bit("时间\n%1\n开盘\n%2\n最高\n%3\n最低\n%4\n收盘\n%5\n成交量\n%6\n涨跌\n%7\n涨跌幅(%)\n%8\n")
                .arg(mydata[currentDay].time).arg(mydata[currentDay].open).arg(mydata[currentDay].high).arg(mydata[currentDay].low)
                .arg(mydata[currentDay].close).arg(mydata[currentDay].volume).arg(mydata[currentDay].close - mydata[currentDay].open)
                .arg(QString::number(100.0* (mydata[currentDay].close - mydata[currentDay].open) / mydata[currentDay].open  , 'f',2));

        painter.drawText(rectText,Qt::AlignCenter, strMessage);
    }
}

void KLineGrid::drawAverageLine(int day)
{
    if( mydata.size() < 1)
        return;

    //y轴缩放
    yscale = getGridHeight() / (highestBid -lowestBid ) ;
    //画笔的线宽
    lineWidth;
    //画线要连接的点
    QVector<QPoint> point;

    //临时点
    QPoint temp;

    //x轴步进
    double xstep = floor((getGridWidth() - 100) / (totalDay - beginDay));   //预留100显示信息
    if( xstep < 2)
        xstep = 2;
    else if(xstep >16)
        xstep = 16;


    switch(day)
    {
    case 5:
        // drawKline中已更新endDay、totalDay
        for( int i= beginDay ;i<totalDay; ++i)
        {
            if( mydata[i].averageLine5 == 0)
                continue;
            temp.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5 * xstep);
            temp.setY(getWidgetHeight() - (mydata[i].averageLine5 - lowestBid) *yscale - getMarginBottom());
            point.push_back(temp);
        }
        break;
    case 10:
        for( int i= beginDay ;i<totalDay;++i)
        {
            if( mydata[i].averageLine10 == 0)
                continue;
            temp.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            temp.setY(getWidgetHeight() - (mydata[i].averageLine10 - lowestBid) *yscale - getMarginBottom());
            point.push_back(temp);
        }
        break;
    case 20:
        for( int i= beginDay ;i<totalDay;++i)
        {
            if( mydata[i].averageLine20 == 0)
                continue;
            temp.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            temp.setY(getWidgetHeight() - (mydata[i].averageLine20 - lowestBid) *yscale - getMarginBottom());
            point.push_back(temp);
        }
        break;
    case 60:
        for( int i= beginDay ;i<totalDay;++i)
        {
            if( mydata[i].averageLine60 == 0)
                continue;
            temp.setX(getMarginLeft() + xstep *(i - beginDay) + 0.5*xstep);
            temp.setY(getWidgetHeight() - (mydata[i].averageLine60 - lowestBid) *yscale - getMarginBottom());
            point.push_back(temp);
        }
        break;
    default:
        break;
    }

    QPainter painter(this);
    QPen     pen;

    switch(day)
    {
    case 5:
        pen.setColor(Qt::white);
        break;
    case 10:
        pen.setColor(Qt::yellow);
        break;
    case 20:
        pen.setColor(Qt::magenta);
        break;
    case 60:
        pen.setColor(Qt::cyan);
        break;
    default:
        pen.setColor(Qt::white);
        break;
    }
    painter.setPen(pen);
    QPolygon polykline(point);
    painter.drawPolyline(polykline);
}

void KLineGrid::paintEvent(QPaintEvent *event)
{
    // 多态，调用父类的绘图事件
    AutoGrid::paintEvent(event);

    // 画K线
    this->drawLine();
}

void KLineGrid::keyPressEvent(QKeyEvent *event)
{
    currentDay = (double)( mousePoint.x() - getMarginLeft() ) / (getGridWidth() -getMarginLeft() - getMarginRight() - 100) * totalDay + beginDay;
    isKeyDown = true;
}

void KLineGrid::mouseMoveEvent(QMouseEvent *event)
{
    mousePoint = event->pos();
    isKeyDown = false;
    update();
}

void KLineGrid::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isCrossShow = !isCrossShow;
        update();
    }
}

void KLineGrid::wheelEvent(QWheelEvent *event)
{

}

void KLineGrid::resizeEvent(QResizeEvent *event)
{
    AutoGrid::resizeEvent(event);
    isCrossShow = false;
}
