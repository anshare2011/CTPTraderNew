#include "klineheader.h"

KLineHeader::KLineHeader(QObject *parent)
{
    this->init();
    this->addConnect();
}

void KLineHeader::init()
{
    kline_type1 = new QPushButton(QString::fromLocal8Bit("K线图"));
    kline_type1->setMinimumWidth(100);
    kline_type1->setMaximumWidth(100);
    kline_type1->setStyleSheet(StyleSheet1 );
    m_current_type = kline_type1;

    kline_type2 = new QPushButton(QString::fromLocal8Bit("分时图"));
    kline_type2->setMinimumWidth(100);
    kline_type2->setMaximumWidth(100);
    kline_type2->setStyleSheet(StyleSheet2 );

    kline_frequency_min1 = new QPushButton(QString::fromLocal8Bit("1m"));
    kline_frequency_min1->setMaximumWidth(60);
    kline_frequency_min1->setStyleSheet(StyleSheet1 );
    m_current_frequency = kline_frequency_min1;

    kline_frequency_min3 = new QPushButton(QString::fromLocal8Bit("3m"));
    kline_frequency_min3->setMaximumWidth(60);
    kline_frequency_min3->setStyleSheet(StyleSheet2 );
    kline_frequency_min5 = new QPushButton(QString::fromLocal8Bit("5m"));
    kline_frequency_min5->setMaximumWidth(60);
    kline_frequency_min5->setStyleSheet(StyleSheet2 );
    kline_frequency_min15 = new QPushButton(QString::fromLocal8Bit("15m"));
    kline_frequency_min15->setMaximumWidth(60);
    kline_frequency_min15->setStyleSheet(StyleSheet2 );
    kline_frequency_min30 = new QPushButton(QString::fromLocal8Bit("30m"));
    kline_frequency_min30->setMaximumWidth(60);
    kline_frequency_min30->setStyleSheet(StyleSheet2 );
    kline_frequency_min60 = new QPushButton(QString::fromLocal8Bit("1h"));
    kline_frequency_min60->setMaximumWidth(60);
    kline_frequency_min60->setStyleSheet(StyleSheet2 );
    kline_frequency_day1 = new QPushButton(QString::fromLocal8Bit("1d"));
    kline_frequency_day1->setMaximumWidth(60);
    kline_frequency_day1->setStyleSheet(StyleSheet2);

    this->setMargin(2);      // 控件与窗体左右边距
    this->setDirection(QBoxLayout::LeftToRight);//设置布局方向
    this->addStretch(1);
    this->addWidget(kline_type1);
    this->addWidget(kline_type2);
    this->addStretch(1);             // 空白占 2/3
    this->addWidget(kline_frequency_min1);
    this->addWidget(kline_frequency_min3);
    this->addWidget(kline_frequency_min5);
    this->addWidget(kline_frequency_min15);
    this->addWidget(kline_frequency_min30);
    this->addWidget(kline_frequency_min60);
    this->addWidget(kline_frequency_day1);
    this->addStretch(6);            // 空白占 1/3
}

void KLineHeader::addConnect()
{
    connect(kline_type1, &QPushButton::clicked, [=](){
        m_current_type->setStyleSheet(StyleSheet2 );
        m_current_type = kline_type1;
        kline_type1->setStyleSheet(StyleSheet1 );
    });

    connect(kline_type2, &QPushButton::clicked, [=](){
        m_current_type->setStyleSheet(StyleSheet2 );
        m_current_type = kline_type2;
        kline_type2->setStyleSheet(StyleSheet1 );
    });


    connect(kline_frequency_min1, &QPushButton::clicked, [=](){
        m_current_frequency->setStyleSheet(StyleSheet2 );
        this->sendKlineType(Min1);
        m_current_frequency = kline_frequency_min1;
        kline_frequency_min1->setStyleSheet(StyleSheet1 );
    });

    connect(kline_frequency_min3, &QPushButton::clicked, [=](){
        m_current_frequency->setStyleSheet(StyleSheet2 );
        this->sendKlineType(Min3);
        m_current_frequency = kline_frequency_min3;
        kline_frequency_min3->setStyleSheet(StyleSheet1 );
    });

    connect(kline_frequency_min5, &QPushButton::clicked, [=](){
        m_current_frequency->setStyleSheet(StyleSheet2 );
        this->sendKlineType(Min5);
        m_current_frequency = kline_frequency_min5;
        kline_frequency_min5->setStyleSheet(StyleSheet1 );
    });

    connect(kline_frequency_min15, &QPushButton::clicked, [=](){
        m_current_frequency->setStyleSheet(StyleSheet2 );
        this->sendKlineType(Min15);
        m_current_frequency = kline_frequency_min15;
        kline_frequency_min15->setStyleSheet(StyleSheet1 );
    });

    connect(kline_frequency_min30, &QPushButton::clicked, [=](){
        m_current_frequency->setStyleSheet(StyleSheet2 );
        this->sendKlineType(Min30);
        m_current_frequency = kline_frequency_min30;
        kline_frequency_min30->setStyleSheet(StyleSheet1 );
    });
}
