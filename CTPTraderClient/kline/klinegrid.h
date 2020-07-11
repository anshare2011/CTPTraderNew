#ifndef KLINEGRID_H
#define KLINEGRID_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QPainter>
#include <QPen>
#include "autogrid.h"
#include "../publicstruct.h"

class KLineGrid : public AutoGrid
{
    Q_OBJECT
public:
    explicit KLineGrid(QWidget *parent = 0);
    void virtual paintEvent(QPaintEvent* event);
    void virtual keyPressEvent(QKeyEvent* event);
    void virtual mouseMoveEvent(QMouseEvent* event);
    void virtual mousePressEvent(QMouseEvent* event);
    void virtual wheelEvent(QWheelEvent *event);
    void virtual resizeEvent(QResizeEvent* event);
    ~KLineGrid();
    void initial();
    void fresh();
    void drawLine();
    void calculateIndicator();
    void drawYtick();
    void drawKline();

    // 带未合成的
    void setMyData(QVector<BarData>& data, BarData data1);

    // 计算均线放在此处，而不是 OneSymbolKlineData 中，避免重复的计算
    void calAverageLine5();
    void calAverageLine10();
    void calAverageLine20();
    void calAverageLine60();

    //键盘没按下画的十字线
    void drawCross2();
    void drawMouseMoveCrossVerLine();
    void drawMouseMoveCrossHorLine();
    void drawTips2();
    void drawDetails();

    //画均线
    void drawAverageLine(int day);

protected:
//    void virtual paintEvent(QPaintEvent* event);
//    void virtual keyPressEvent(QKeyEvent* event);
//    void virtual mouseMoveEvent(QMouseEvent* event);
//    void virtual mousePressEvent(QMouseEvent* event);
//    void virtual wheelEvent(QWheelEvent *event);
//    void virtual resizeEvent(QResizeEvent* event);

signals:

public slots:

private:
    bool is_unfinished = false;
    // 临时K线数据
    QVector<BarData> mydata;
    BarData unfinishedBar;


    // K线宽度显示90%
    double smallPercent;

    //画k线的起始日期和终止日期
    int beginDay;
    int endDay;
    int totalDay;
    int currentDay;

    //当前要画的k线日期中的最高价，最低价，最大成交量
    double highestBid;
    double lowestBid;
    double maxVolume;

    //x轴和y轴的缩放比
    double xscale;
    double yscale;

    //鼠标位置
    QPoint mousePoint;

    //画笔的线宽
    int lineWidth;

    //是否显示十字线
    bool isCrossShow = false;

    //键盘是否按下
    bool isKeyDown = false;

    //是否画均线
    bool isDrawAverageLine = true;

    // 是否调用init方法初始化,第一次进来以后就init了
    bool isInit = false;
};

#endif // KLINEGRID_H
