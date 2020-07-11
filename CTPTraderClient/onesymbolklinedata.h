#ifndef ONESYMBOLKLINEDATA_H
#define ONESYMBOLKLINEDATA_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <memory>
#include <QVector>
#include <QSet>
#include "publicstruct.h"

// 一个合约 的K线数据对象类，保存一个标的K线不同频率的数据（本地收集）

class OneSymbolKlineData : public QObject
{
    Q_OBJECT
public:
    OneSymbolKlineData() = delete;
    OneSymbolKlineData(QString symbol);

    ~OneSymbolKlineData();

    // 更新 tick数据
    void updateQuoteInformation(std::shared_ptr<QuoteInfo> quote);

    // 跟新完整K线
    void updateMin1Bar(BarData& bar);

    void updateMinBar(int min_num, QVector<BarData>&vertor_bar, BarData&temp_bar, BarData& bar);
    void updateMin3Bar(BarData& bar);
    void updateMin5Bar(BarData& bar);

    // 跟新临时K线
    void updateMin3Temp();
    void updateMin5Temp();

    // 更新 bar数据, 给 OneFrequencyKlineData 类型
    void updateBarData(BarData& bar, QString frequency);

signals:

public slots:
    void receiveKlineBarData(KlineType, QVector<BarData>);

public:
    QSet<KlineType> is_get_kline_data;              // 该频率的历史数据 是否获取
    QString m_symbol;
    QString m_time;                                 // 最新时间
    const double small_number = 0.000000001;
    double upperLimitPrice = 0;                     // 当天的涨跌停
    double lowerLimitPrice = 0;
    double m_lastPrice = 0;

    QVector<BarData> min1Data;
    QVector<BarData> min3Data;
    QVector<BarData> min5Data;
    QVector<BarData> min15Data;
    QVector<BarData> min30Data;
    QVector<BarData> hour1Data;
    QVector<BarData> day1Data;
    BarData min1temp;
    BarData min3temp;
    BarData min5temp;
    BarData min15temp;
    BarData min30temp;


    OneFrequencyKlineData * min1_Data{nullptr};
    OneFrequencyKlineData * min3_Data{nullptr};
    OneFrequencyKlineData * min5_Data{nullptr};
    OneFrequencyKlineData * min15_Data{nullptr};
    OneFrequencyKlineData * min30_Data{nullptr};
    OneFrequencyKlineData * hour1_Data{nullptr};
    OneFrequencyKlineData * day1_Data{nullptr};
};

#endif // ONESYMBOLKLINEDATA_H
