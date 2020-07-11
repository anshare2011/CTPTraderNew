#ifndef ONESYMBOLKLINEDATA_H
#define ONESYMBOLKLINEDATA_H

#include <QString>
#include <QDateTime>
#include <memory>
#include <QVector>
#include "publicstruct.h"
#include "savedatetodatabase.h"

class OneSymbolKlineData
{
public:
    OneSymbolKlineData() = delete;
    OneSymbolKlineData(QString symbol, QString exchange_name);
    ~OneSymbolKlineData();

    void updateQuoteInformation(std::shared_ptr<QuoteInfo> quote);

private:
    void updateMin1Bar(BarData& bar);

    void updateMinBar(int min_num, BarData&temp_bar, BarData& bar);
    void updateMin30Bar(BarData& bar);
    void updateHour1Bar(BarData& bar);
    void updateDay1Bar(BarData& bar);

    void updateTickBar(std::shared_ptr<QuoteInfo> &);

private:
    QString m_symbol;
    QString m_exchange_name;
    const double small_number = 0.000000001;
    SaveDateToDataBase *m_save_data{nullptr};

    // 只需要临时的bar，跳bar以后立刻存数据库并生成临时的bar
    BarData temp_min1;
    BarData temp_min3;
    BarData temp_min5;
    BarData temp_min15;
    BarData temp_min30;
    BarData temp_hour1;
    BarData temp_day1;

    QList<std::shared_ptr<QuoteInfo> > quote_list;
};

#endif // ONESYMBOLKLINEDATA_H
