#ifndef KLINECHART_H
#define KLINECHART_H

#include <QWidget>
#include <QDebug>
#include <QTabWidget>
#include <QString>
#include <QList>
#include <QPoint>
#include <QMenu>
#include <QPaintEvent>
#include <QTableWidget>
#include <QScrollBar>
#include <QHeaderView>
#include <QSplitter>
#include "mainengine.h"
#include "klinewidget.h"
#include "publicstruct.h"
#include "onesymbolklinedata.h"
#include "mywidgets/mytabwidget.h"
#include "ui/futuretabletop.h"

extern MainEngine* m_me;

// 主界面 上部分 的图表控件

class KlineChart : public MyTabWidget
{
    Q_OBJECT
public:
    explicit KlineChart(QWidget *parent = 0);

    void klineInit();

    void initFutureTable();
    void initOptionTable();

    void setCurrentFrequency(QString frequency);

    void addWidget1(QString symbol, QWidget* w);

    void clearOptionTable();


signals:
    // 告诉下单板 当前的K线合约
    // void sendCurrentSymbolToTradeboard(QString symbol);

public slots:
    void setCurrentSymbol(QString symbol);

    void receiveKlineInformation(QString symbol, OneSymbolKlineData*);

    void receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote);

    void SLOTMainCanvas(QPoint);

    void slot_myfun();

    void doubleClicked(int,int);

    void currentIndexChanged(int index);
    // 期权也更新选中的期权标的
    void receiveSelectedOption(QString);
    // 接收到快捷下单
    void receiveQuickType(QuickType);
    // 接收期货行情
    void receiveOptionQuoteInformation(std::shared_ptr<QuoteInfo> quote);

protected:
    // 画边框
    void paintEvent(QPaintEvent *event);
    // 鼠标点击事件
    void mousePressEvent(QMouseEvent *event);
    // 回车事件
//    void keyPressEvent(QKeyEvent *event);

//    bool eventFilter(QObject *target, QEvent *event) override;

private:
    // 所有推送数据过来的合约
    QSet<QString> all_instruments;                              // 接收到的行情集合
    // symbol: KlineWidget* 对象
    QMap<QString, KlineWidget*>	all_klineWidget;
    // 保存数据指针
    QMap<QString, OneSymbolKlineData *>	all_symbol_data;        // 保存最新的K线数据
    QMap<QString, int> symbol_row;

    QString currentFrequency;      // 当前频率
    // QString currentSymbol ;        // 当前标的名称

    QTableWidget *table_future{nullptr};
    QStringList name_future;
    const QString future_name = QString::fromLocal8Bit("期货主力合约");

    QTableWidget *table_option{nullptr};
    QStringList name_option;
    const QString option_name = QString::fromLocal8Bit("期权");

    int quote_count = 0;   // 当前显示第一行的行数
    int future_row_count = 0;
    const int cycle_num = 10;

    FutureTableTop *m_future_top = nullptr;
};

#endif // KLINECHART_H
