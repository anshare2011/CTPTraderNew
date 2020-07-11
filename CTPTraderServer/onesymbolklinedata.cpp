#include "onesymbolklinedata.h"


OneSymbolKlineData::OneSymbolKlineData(QString symbol, QString exchange_name):m_symbol(symbol), m_exchange_name(exchange_name)
{
//    m_symbol = symbol;
//    m_exchange_name = exchange_name;
    QString database_name = exchange_name +".db";
    m_save_data = new SaveDateToDataBase(database_name);
}

OneSymbolKlineData::~OneSymbolKlineData()
{

}

void OneSymbolKlineData::updateQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    // qDebug()<< "OneSymbolKlineData::updateQuoteInformation"<< quote->symbol<< quote->time<< quote->volume;
    if(quote->volume <1)
        return;
    this->updateTickBar(quote);

    int timeMinute = quote->time.split(":").at(1).toInt();      // tick当前分钟 , 11:20:17--->20
    double last_price = quote->lastPrice;                       // 最新价
    BarData& bar = temp_min1;
    if (timeMinute != bar.currentMinute)                        // 每个分钟的第一个 tick
    {
        if (!bar.is_new)
        {
            // 将完整的旧bar推走
            this->updateMin1Bar(bar);
        }
        // 初始化新ba
        bar = BarData();
        bar.symbol = quote->symbol;
        bar.tradingDay = quote->tradingDay;
        bar.begin_time = quote->time;              // K线的时间设为第一个Tick的时间
        bar.end_time = quote->time;
        bar.open = last_price;
        bar.high = last_price;
        bar.low = last_price;
        bar.close = last_price;
        bar.begin_volume = quote->volume;
        bar.end_volume = quote->volume;
        bar.volume = 0;
        bar.currentMinute = timeMinute;
        bar.is_new = false;
    }
    else
    {
        // 刷新1分钟临时bar数据
        bar.is_one_tick = false;
        bar.high = (bar.high - last_price >small_number) ? bar.high : last_price;
        bar.low = (bar.low - last_price < small_number) ? bar.low : last_price;
        bar.close = last_price;
        bar.end_volume = quote->volume;
        bar.volume = bar.end_volume - bar.begin_volume;
        bar.end_time = quote->time;
    }
}

void OneSymbolKlineData::updateMin1Bar(BarData &bar)
{
    if(bar.is_one_tick)             // 单个tick 过滤掉
        return;
    // 将min1 bar保存到本地数据库
    QString table_name = bar.symbol + "_min1";
    m_save_data->addBarData(table_name, bar);

    this->updateMinBar(3, temp_min3, bar);
    this->updateMinBar(5, temp_min5, bar);
    this->updateMinBar(15, temp_min15, bar);
//    this->updateMinBar(30, temp_min30, bar);

    this->updateMin30Bar(bar);
    this->updateHour1Bar(bar);
    this->updateDay1Bar(bar);
}

void OneSymbolKlineData::updateMinBar(int min_num, BarData &temp_bar, BarData &bar)
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

        // 将min3 bar保存到本地数据库
        QString table_name = bar.symbol + "_min" + QString::number(min_num);
        m_save_data->addBarData(table_name, temp_bar);

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

// 早上交易时间 09：00->  10：15，没有29分钟的K线,
void OneSymbolKlineData::updateMin30Bar(BarData &bar)
{

}

void OneSymbolKlineData::updateHour1Bar(BarData &bar)
{

}

void OneSymbolKlineData::updateDay1Bar(BarData &bar)
{

}

void OneSymbolKlineData::updateTickBar(std::shared_ptr<QuoteInfo> &quote)
{
    QString table_name = quote->symbol + "_tick";
    // m_save_data->addTickData(table_name, quote);

    if(quote_list.size() < 200){
        quote_list.append(quote);
        return;
    }
    else{
        quote_list.append(quote);
        if(m_save_data->addTickData(table_name, quote_list))
        {
            quote_list.clear();
        }
    }

}
