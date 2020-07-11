#ifndef AUTOGRID_H
#define AUTOGRID_H

#include <QWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

//  https://github.com/cetcjinjian/StockKLine


class AutoGrid : public QWidget
{
    Q_OBJECT
public:
    explicit AutoGrid(QWidget *parent = 0);
    void virtual initial();
    void virtual drawBackground();
    void virtual calAtomGridHeight();
    void virtual calAtomGridWidth();
    void virtual resizeEvent(QResizeEvent* event);
    void virtual paintEvent(QPaintEvent* event);

    void drawGrid();
    void drawHorLine();
    void drawVerLine();

    int getMarginLeft() const;
    void setMarginLeft(int value);

    int getMarginRight() const;
    void setMarginRight(int value);

    int getMarginTop() const;
    void setMarginTop(int value);

    int getMarginBottom() const;
    void setMarginBottom(int value);

    int getWidgetHeight() const;
    int getWidgetWidth() const;

    double getGridHeight() const;
    double getGridWidth() const;

    int getHGridNum() const;
    void setHGridNum(int value);

    int getWGridNum() const;
    void setWGridNum(int value);

    double getAtomGridHeightMin() const;
    void setAtomGridHeightMin(double value);

    double getAtomGridWidthMin() const;
    void setAtomGridWidthMin(double value);

    double getAtomGridHeight() const;
    double getAtomGridWidth() const;


private:
    //表格距边框距离
    int marginLeft      = 20;
    int marginRight     = 60;
    int marginTop       = 0;
    int marginBottom    = 0;

    //当前widget的宽度和高度
    int widgetHeight;
    int widgetWidth;

    //当前表格的宽度和高度
    double gridHeight;
    double gridWidth;

    //当前表格中最小表格的宽度和高度
    double atomGridHeight;
    double atomGridWidth;

    //表格中小格子的数量
    int hGridNum;
    int wGridNum;

    //当前表格中最小表格的宽度和高度的最小值
    double atomGridHeightMin;
    double atomGridWidthMin;

signals:

public slots:

};

#endif // AUTOGRID_H
