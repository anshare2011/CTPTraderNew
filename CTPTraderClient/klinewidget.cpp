#include "klinewidget.h"
#include <QPushButton>
#include <QComboBox>
#include <QToolButton>
#include <QHBoxLayout>

KlineWidget::KlineWidget(int x, int y, QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("font:12pt '宋体';");

    m_header = new KLineHeader();

    pkline = new KLineGrid();
    pkline->setFocusPolicy(Qt::StrongFocus);

    pvolume = new kVolumeGrid();
    pvolume->setFocusPolicy(Qt::StrongFocus);
    pvolume->setMaximumHeight(180);

    m_trade_detail = new Tradeboard();

    splitterMain = new QSplitter(Qt::Horizontal, this);      //新建主分割窗口，水平分割
    splitterMain->setGeometry(0, 0, x, y);
    QSplitter *left = new QSplitter(Qt::Vertical, splitterMain);
    QSplitter *right = new QSplitter(Qt::Vertical, splitterMain);

    QSplitter *splitter1 = new QSplitter(Qt::Vertical, left);
    QSplitter *splitter2 = new QSplitter(Qt::Vertical, left);
    QSplitter *splitter3 = new QSplitter(Qt::Vertical, left);

    //    splitterMain->setHandleWidth(1);
    //    splitter1->addWidget(pkline);
    //    splitter2->addWidget(pvolume);

    QWidget *cornerWidget = new QWidget;
    cornerWidget->setMaximumHeight(40);
    cornerWidget->setLayout(m_header);

    splitter1->addWidget(cornerWidget);
    splitter2->addWidget(pkline);
    splitter3->addWidget(pvolume);
    left->setHandleWidth(1);
    splitterMain->setHandleWidth(1);

    right->addWidget(m_trade_detail);

    splitterMain->setStretchFactor(0,2);
    splitterMain->setStretchFactor(1,1);              //设置比例不起作用


    this->addConnect();
}

void KlineWidget::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    m_trade_detail->receiveQuoteInformation(quote);
}

void KlineWidget::updateKline(OneSymbolKlineData *data)
{
    this->m_data = data;

    if(this->type == KlineType::Min1)
    {
        pkline->setMyData(data->min1Data, data->min1temp);
        pvolume->setMyData(data->min1Data, data->min1temp);

    }
    else if(this->type == KlineType::Min3)
    {
        if(data->min3temp.is_new)
        {
            pkline->setMyData(data->min3Data, data->min1temp);
            pvolume->setMyData(data->min3Data, data->min1temp);
        }
        else
        {
            pkline->setMyData(data->min3Data, data->min3temp + data->min1temp);
            pvolume->setMyData(data->min3Data, data->min3temp + data->min1temp);
        }
    }
    else if(this->type == KlineType::Min5)
    {
        if(data->min5temp.is_new)
        {
            pkline->setMyData(data->min5Data, data->min1temp);
            pvolume->setMyData(data->min5Data, data->min1temp);
        }
        else
        {
            pkline->setMyData(data->min5Data, data->min5temp + data->min1temp);
            pvolume->setMyData(data->min5Data, data->min5temp + data->min1temp);
        }
    }
    else if(this->type == KlineType::Min15)
    {
        if(data->min15temp.is_new)
        {
            pkline->setMyData(data->min15Data,data->min1temp);
            pvolume->setMyData(data->min15Data, data->min1temp);
        }
        else
        {
            pkline->setMyData(data->min15Data, data->min15temp + data->min1temp);
            pvolume->setMyData(data->min15Data, data->min15temp + data->min1temp);
        }
    }
    else if(this->type == KlineType::Min30)
    {
        if(data->min30temp.is_new)
        {
            pkline->setMyData(data->min30Data, data->min1temp);
            pvolume->setMyData(data->min30Data, data->min1temp);
        }
        else
        {
            pkline->setMyData(data->min30Data, data->min30temp + data->min1temp);
            pvolume->setMyData(data->min30Data, data->min30temp + data->min1temp);
        }
    }
    else
    {
        pkline->setMyData(data->min1Data, data->min1temp);
        pvolume->setMyData(data->min1Data, data->min1temp);

    }
}

void KlineWidget::addConnect()
{
    connect(m_header, &KLineHeader::sendKlineType, [=](KlineType type){
        this->type = type;
        this->updateKline(m_data);
    });
}

void KlineWidget::receiveQuickType(QuickType type)
{
    m_trade_detail->receiveQuickType(type);
}

void KlineWidget::resizeEvent(QResizeEvent *event)
{
    int x = this->width();
    int y = this->height();
    splitterMain->setGeometry(0, 0, x, y);

    splitterMain->setStretchFactor(0,2);
    splitterMain->setStretchFactor(1,1);
}
