#ifndef KLINEWIDGET_H
#define KLINEWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QResizeEvent>
#include "kline/klinegrid.h"
#include "kline/kvolumegrid.h"
#include "publicstruct.h"
#include "onesymbolklinedata.h"
#include "klineheader.h"
#include "tradeboard.h"
#include "publicstruct.h"

// 显示一张K线图表，插入到KlineChart tabwidget 表中

class KlineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KlineWidget(int x, int y, QWidget *parent = 0);

    void receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote);

    void updateKline(OneSymbolKlineData *);

    void addConnect();

    void receiveQuickType(QuickType type);

protected:
    void resizeEvent(QResizeEvent* event);

signals:

public slots:

private:
    QSplitter *splitterMain;
    KLineHeader *m_header;
    KLineGrid* pkline;
    kVolumeGrid* pvolume;
    Tradeboard *m_trade_detail = nullptr;

    OneSymbolKlineData * m_data = nullptr;

    enum KlineType type = KlineType::Min1;
};

#endif // KLINEWIDGET_H
