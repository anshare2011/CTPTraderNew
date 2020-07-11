#ifndef KLINEHEADER_H
#define KLINEHEADER_H

#include <QObject>
#include <QHBoxLayout>
#include <QPushButton>
#include "publicstruct.h"

// K线上方的鼠标控件，

class KLineHeader : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit KLineHeader(QObject *parent = 0);

    void init();

    void addConnect();

signals:
    void sendKlineType(KlineType);

public slots:

private:
    QPushButton *kline_type1;
    QPushButton *kline_type2;
    QPushButton *m_current_type = nullptr;

    QPushButton *m_current_frequency = nullptr;
    QPushButton *kline_frequency_min1;
    QPushButton *kline_frequency_min3;
    QPushButton *kline_frequency_min5;
    QPushButton *kline_frequency_min15;
    QPushButton *kline_frequency_min30;
    QPushButton *kline_frequency_min60;
    QPushButton *kline_frequency_day1;

    const QString StyleSheet1 =  "QPushButton{background-color: rgb(0, 255, 255);}";    // https://tool.oschina.net/commons?type=3
    const QString StyleSheet2 =  "QPushButton{background-color: rgb(255, 255, 0); color:black; font-weight: bold;font-family:Times }";
};

#endif // KLINEHEADER_H
