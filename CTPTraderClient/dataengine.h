#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <QObject>
#include <QDebug>
#include <memory>
#include "publicstruct.h"
#include "onesymbolklinedata.h"
#include "getdatafromserver.h"

// 1.维护K线数据，由tick合成分钟


class DataEngine : public QObject
{
    Q_OBJECT
public:
    explicit DataEngine(QObject *parent = 0);
    // 更新合约相关信息
    void updataInstrumentInfo(InstrumentInfo& info);

    void addSubscribeInstrument(QString instrument);

    // 获取K线数据
    OneSymbolKlineData* returnOneSymbolKlineData(QString);

    // 获取数据
    void getKlineDataFromSerrver(QString symbol, KlineType type, int size = 200);

signals:
    void sendQuoteInformation(std::shared_ptr<QuoteInfo>);

    void sendKlineInformation(QString symbol, OneSymbolKlineData*);

public slots:
    void receiveQuoteInformation(std::shared_ptr<QuoteInfo>);
    void receiveKlineBarData(QString, KlineType, QVector<BarData>);

private:
    void updateQuoteAndSendToMainwindow(QString symbol, std::shared_ptr<QuoteInfo> quote);

private:
    QMap<QString, OneSymbolKlineData*> all_symbol_data;     // 合约：K线数据指针
    QMap<QString, QString> symbol_name;
    QMap<QString, QString> symbol_exchangeId;

    QSet<QString> subscribe_instrument;       // 主动订阅的合约集合
};

#endif // DATAENGINE_H
