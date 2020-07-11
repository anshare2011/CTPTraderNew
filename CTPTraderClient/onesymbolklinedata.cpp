#include "onesymbolklinedata.h"
#include <QDebug>

OneSymbolKlineData::OneSymbolKlineData(QString symbol)
{
    min1Data.reserve(720);
    min3Data.reserve(240);
    min5Data.reserve(144);

    this->m_symbol = symbol;
}

OneSymbolKlineData::~OneSymbolKlineData()
{

}

void OneSymbolKlineData::updateQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    m_lastPrice = quote->lastPrice;
    m_time = quote->time;

    QString  stringMinute = quote->time.split(":").at(1);      // 时间格式 11:20:17,获得行情的分钟数 20
    int timeMinute = stringMinute.toInt();
    double last_price = quote->lastPrice;                       // 最新价
    BarData& bar = min1temp;
    if (timeMinute != bar.currentMinute)                        // 每个分钟的第一个 tick
    {
        if (!bar.is_new)
        {
            // 将完整的旧bar推走, 并初始化新bar
            updateMin1Bar(bar);
        }
        bar = BarData();
        bar.symbol = quote->symbol;
        bar.tradingDay = quote->tradingDay;
        bar.time = quote->time;              // K线的时间设为第一个Tick的时间
        bar.open = last_price;
        bar.high = last_price;
        bar.low = last_price;
        bar.close = last_price;

        bar.volume_begin = quote->volume;
        bar.volume_end = quote->volume;
        bar.volume = 0;
        bar.currentMinute = timeMinute;     // 更新 min1temp bar 分钟的时间
        bar.is_new = false;
        bar.is_one_tick = true;

//        // 刷新3分钟临时bar数据
//        updateMin3Temp();
//        // 刷新5分钟临时bar数据
//        updateMin5Temp();
    }
    else
    {
        // 刷新1分钟临时bar数据
        bar.is_one_tick = false;
        bar.high = (bar.high - last_price >small_number) ? bar.high : last_price;
        bar.low = (bar.low - last_price < small_number) ? bar.low : last_price;
        bar.close = last_price;
        bar.volume_end = quote->volume;
        bar.volume = bar.volume_end - bar.volume_begin;

//        // 刷新3分钟临时bar数据
//        updateMin3Temp();
//        // 刷新5分钟临时bar数据
//        updateMin5Temp();
    }
}

void OneSymbolKlineData::updateMin1Bar(BarData &bar)
{
    if(bar.is_one_tick)             // 单个tick 过滤掉
        return;

    // 将1分钟数据加入到min1Data
    min1Data.push_back(bar);
    this->updateMinBar(3, min3Data, min3temp, bar);
    this->updateMinBar(5, min5Data, min5temp, bar);
    this->updateMinBar(15, min15Data, min15temp, bar);
    this->updateMinBar(30, min30Data, min30temp, bar);
//    updateMin3Bar(bar);
//    updateMin5Bar(bar);
}

void OneSymbolKlineData::updateMinBar(int min_num, QVector<BarData> &vertor_bar, BarData &temp_bar, BarData &bar)
{
    int currentMin = bar.currentMinute;

    if((currentMin+1) % min_num == 0)
    {
        if(temp_bar.is_new){
            temp_bar = bar;
            temp_bar.is_new = false;
        }
        else
            temp_bar = temp_bar + bar;

        // 将合成的数据加入
        vertor_bar.push_back(temp_bar);
        temp_bar = BarData();
    }
    else{
        if(temp_bar.is_new){
            temp_bar = bar;
            temp_bar.is_new = false;
        }
        else
            temp_bar = temp_bar + bar;
    }
}

void OneSymbolKlineData::updateMin3Bar(BarData &bar)
{
    // 判断1分钟bar是不是3分钟（6-8）的最后1分钟，是的话合成一个完整的K线加入到min3Data中
    int currentMin = bar.currentMinute;
    if((currentMin+1) %3 == 0)    // 0-2, 9-11   参见 KlineTab
    {
        int size = min1Data.size();
        int num = (size >3)? 3: size;
        int begin = (num >=3)? size-3:0;
        int end = size;
        BarData bar3 = min1Data.at(begin);
        for(int i= 1+begin; i< end; ++i)
        {
            bar3 = bar3 + min1Data.at(i);
        }
        min3Data.push_back(bar3);
    }
}

void OneSymbolKlineData::updateMin5Bar(BarData &bar)
{
    // 判断1分钟bar是不是5分钟（5-9）的最后1分钟，是的话合成一个完整的K线加入到min5Data中
    int currentMin = bar.currentMinute;
    if((currentMin + 1) %5 == 0)     // 10-14
    {
        int size = min1Data.size();
        int num = (size >5)? 5: size;
        int begin = (num >=5)? size-5:0;
        int end = size;
        BarData bar5 = min1Data.at(begin);
        for(int i= 1+begin; i< end; ++i)
        {
            bar5 = bar5 + min1Data.at(i);
        }
        min5Data.push_back(bar5);
    }
}

void OneSymbolKlineData::updateMin3Temp()
{
    min3temp = min1temp;
    int add_num = min1temp.currentMinute %3;
    if(add_num >0)
    {
        int can_add_num = (min1Data.size() >add_num)? add_num: min1Data.size();
        for(int i = 0; i<can_add_num; ++i)
        {
            BarData temp = min1Data.at(min1Data.size()-i -1);
            min3temp = temp + min3temp;
        }
    }
}

void OneSymbolKlineData::updateMin5Temp()
{
    min5temp = min1temp;
    int add_num = min1temp.currentMinute %5;
    if(add_num >0)
    {
        int can_add_num = (min1Data.size() >add_num)? add_num: min1Data.size();
        for(int i = 0; i<can_add_num; ++i)
        {
            BarData temp = min1Data.at(min1Data.size()-i -1);
            min5temp = temp + min5temp;
        }
    }
}

void OneSymbolKlineData::updateBarData(BarData &bar, QString frequency)
{

}

void OneSymbolKlineData::receiveKlineBarData(KlineType type, QVector<BarData> klinedata)
{
    if(is_get_kline_data.contains(type))
        return;
    is_get_kline_data.insert(type);

    for(int i =0; i< klinedata.size(); ++i)
    {
        const BarData& data = klinedata.at(i);

        if(type == KlineType::Min1)
            this->min1Data.push_front(data);
        else if(type == KlineType::Min3)
            this->min3Data.push_front(data);
        else if(type == KlineType::Min5)
            this->min5Data.push_front(data);
        else if(type == KlineType::Min15)
            this->min15Data.push_front(data);
    }
}
