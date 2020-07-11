#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QSet>
#include <QDebug>
#include <memory>
#include "onesymbolklinedata.h"

class DataEngine : public QObject
{
    Q_OBJECT
public:
    explicit DataEngine(QObject *parent = 0) = delete;
    DataEngine(QString exchange_name);

signals:

public slots:
    void receiveQuoteInformation(std::shared_ptr<QuoteInfo>);

private:
    QMap<QString, OneSymbolKlineData*> all_symbol_data;     // 合约：K线数据指针
    QString exchange_name = "";
};

#endif // DATAENGINE_H
