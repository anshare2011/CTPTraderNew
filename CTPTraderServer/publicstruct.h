#ifndef PUBLICSTRUCT_H
#define PUBLICSTRUCT_H

#include <QString>
#include <QDateTime>
#include <memory>
#include <QVector>

typedef  enum KlineType {
    Min1=1,
    Min3=3,
    Min5=5,
    Min15=15,
    Min30=30,
    Min60=60,
    Day1=24*60,
}KlineType;

//记录登录的基本信息：用户ID，交易账号，账号密码，经纪商代码，前置机地址
struct LoginInformation
{
    QString mdAddress;
    QString tdAddress;
    QString brokerID;
    QString userName;
    QString password;

    QString AppID;
    QString productionInfo;
    QString authorizationCode;
};

// Tick行情信息
struct QuoteInfo
{
    int volume{0};
    int openInterest{0};
    int holdingAmount{0};
    int bidVolume1{0};
    int askVolume1{0};

    double openPrice{0};
    double highPrice{0};
    double lowPrice{0};
    double lastPrice{0};

    double bidPrice1{0};    // 买一价
    double askPrice1{0};    // 卖一价

    double upperLimit{0};   // 涨停
    double lowerLimit{0};   // 跌停

    double preClosePrice{0};         // 昨收
    double preSettlementPrice{0};    // 昨结

    QString symbol;
    QString exchangeid;  // 交易所

    QString time;    // 时间
    QString tradingDay;
    QString gatewayName;
};

// 成交信息
struct TradeInfo
{
    //
    QString userId;

    // 代码编号相关
    QString symbol;
    QString vtSymbol;
    QString exchange;

    QString tradeID;
    QString vtTradeID;

    QString orderID;
    QString vtOrderID;

    // 成交相关
    QChar direction;
    QChar offset;
    double price;
    int volume;
    QString tradeTime;

    QString gatewayName;
    QChar tradeway;     // 发单类型
};

// 委托单信息
struct OrderInfo
{
    QString userId;
    // 代码编号相关
    QString symbol;
    QString vtSymbol;
    QString exchange;
    QString orderID;
    QString vtOrderID;

    // 报单相关
    QChar direction;    //'0'买   '1'卖
    QChar offset;
    double price;
    int totalVolume;
    int tradeVolume;
    QChar status;
    QString orderTime;
    QString cancelTime;
    int frontID;
    int sessionID;
    QString gatewayName;

    QChar tradeway;     // 发单类型
};

// 持仓信息
struct PositionInfo
{
    // 考虑到现阶段大部分CTP中的ExchangeID字段返回的都是空值, vtSymbol直接使用symbol
    QString userId;
    QString symbol;
    QString vtSymbol;

    QString directName;
    QChar direction{ 0 };
    int position{ 0 };

    int frozen{ 0 };
//    double price{ 0 };   // 由开仓价值和均价替代

    QString vtPositionName;
    int ydPosition{ 0 };
    int tdPosition{ 0 };

    QString gatewayName;

    int multiplier;
    QString exchangeId;

    double openCost{ 0 };   // 合约价值
    double averagePrice{ 0 };  // 均价
};

// 用户帐号资金账户信息
struct AccountInfo
{
    // 帐号相关
    QString id = "";
    QString gatewayName = "";

    // 数值相关
    double preBalance {0};
    double balance {0};
    double available {0};
    double commission {0};  //手续费
    double margin {0};     //当前保证金总额
    double close_profit {0};
    double position_profit {0};
};


// 下单板信息结构体
struct BoardInfo
{
    double upperprice;
    double ask1price;
    double bid1price;
    double lowerprice;
    QString exchangeid;
    QString instrument;
    QString userid;

};

// 下单命令请求
struct orderCommonRequest
{
    QString instrument;
    double price;
    int volume;
    char pricetype;   // '0'表示市价， '1'表示限价
    char direction;   // '0' 买 '1'买
    char offset;      // '0'开 '1'平
    char tradeWay;     // 下单方式，'0'表示手动、'1'表示快捷、'2'表示策略、 '3'手动平仓、'4'表示远程, 5'止盈止损'
    QString exchangeid;
    QString userid;
};

//取消命令请求
struct cancelCommonRequest
{
    QString instrument;
    QString exchange;

    // CTP, LTS相关
    QString order_ref;
    int session_id;
    int front_id;
};


// K线Bar数据
struct BarData
{
    void operator=(const BarData &data)
    {
        this->symbol = data.symbol;
        this->exchange = data.exchange;
        this->begin_time = data.begin_time;
        this->end_time = data.end_time;
        this->time = data.time;
        this->tradingDay = data.tradingDay;
        this->open = data.open;
        this->high = data.high;
        this->low = data.low;
        this->close = data.close;

        this->volume = data.volume;
        this->begin_volume = data.begin_volume;
        this->end_volume = data.end_volume;
        this->currentMinute = data.currentMinute;
        this->is_new = data.is_new;
        this->is_one_tick = data.is_one_tick;
    }
    BarData operator +(const BarData &data1)
    {
        BarData temp;
        temp.symbol = this->symbol;
        temp.exchange = this->exchange;
        temp.begin_time = this->begin_time;
        temp.end_time = data1.end_time;

        temp.time = this->time;
        temp.tradingDay = this->tradingDay;
        temp.open = this->open;
        temp.high = (this->high > data1.high) ? this->high: data1.high;
        temp.low = (this->low < data1.low) ? this->low: data1.low;
        temp.close = data1.close;

        temp.begin_volume = this->begin_volume;
        temp.end_volume = data1.end_volume;
        temp.volume = (temp.end_volume > temp.begin_volume)? temp.end_volume - temp.begin_volume: 0;
        temp.currentMinute = this->currentMinute;
        temp.is_new = false;
        temp.is_one_tick = false;
        return temp;
    }

    QString symbol = "";
    QString exchange = "";
    QString begin_time = "";
    QString end_time = "";
    QString time = "";
    QString tradingDay = "";

    double open{ 0 };
    double high{ 0 };
    double low{ 0 };
    double close{ 0 };

    // 在画图的时候计算
    double averageLine5{ 0 };            //5日均线
    double averageLine10{ 0 };           //10日均线
    double averageLine20{ 0 };
    double averageLine60{ 0 };
    double volumeAverage5{ 0 };          //成交量5日均值
    double volumeAverage10{ 0 };         //成交量10日均值

    int begin_volume{ 0 };               // 当前bar 收到第一个tick时的成交量
    int end_volume{0};                   // 当前bar 最后一个tick收到的成交量
    int volume{ 0 };
    int currentMinute{ -1 };
    bool is_new{ true };
    bool is_one_tick{ true };               // 是否由一个tick组成
};


struct TickData{
    int begin_volume{ 0 };
    int end_volume{0};
    double open{ 0 };
    double high{ 0 };
    double low{ 0 };
    double close{ 0 };
    char begin_time[16];
    char end_time[16];
    char tradingDay[16];
    char symbol[32];
};

struct OneFrequencyKlineData{
    OneFrequencyKlineData() = delete;
    OneFrequencyKlineData(QString symbol, QString frequence){
        this->symbol = symbol;
        this->frequence = frequence;
    }

    void updateBarData(BarData& bar){
        size += 1;
        ask1_price = bar.close;
        bid1_price = bar.close;
        last_price = bar.close;
        open.push_back(bar.open);
        high.push_back(bar.high);
        low.push_back(bar.low);
        close.push_back(bar.close);
        volume.push_back(bar.volume);
    }

    int size = 0;
    QString symbol;
    QString frequence;
    double ask1_price;
    double bid1_price;
    double last_price;
    std::vector<double> open;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> close;
    std::vector<double> volume;
};

//合约信息：id、名字、交易所编号、最后交割日、保证金率、合约乘数、保证金率、手续费、最小变动单位、现在是否可以交易
struct InstrumentInfo
{
public:
    //为了放在set集合中，重载 < 运算符
    bool operator < (const InstrumentInfo &info) { return symbol < info.symbol; }

public:
    QString symbol;    // instrument
    QString name;
    QString exchangeId;
    QString ProductID;
    QDate deadline;

    double marginRate {0};
    int multiplier {0};
    double openCommission {0};
    double closeCommission {0};
    double closeTodayCommission {0};
    double minimumUnit {0};
    bool tradable { false };
    bool has_subsribed { false };
    bool is_end{ false };
    double openInterest {0};
    double tempVolume {0};
};

// 保存一个账号的委托、持仓、历史成交、账号等信息
struct stopWinLossOrder{
    bool operator ==(const stopWinLossOrder& s1)
    {
        if(this->number == s1.number && this->symbol == s1.symbol && this->direction == s1.direction)
            return true;
        else
            return false;
    }


    int number{-1};   // 编号，数字标识
    QString time;
    QString user_id;
    QString symbol;
    QChar direction;   //原始下单方向   0买 1卖
    QChar status;    // 0运行中， 1已取消，2已触发
    QChar type;    // 0止盈 1止损
    double stop_price{0};   // 触发价
    int amount{0};   // 数量
    int multiplier {0};   // 乘数，计算盈亏
    QChar stop_orde_type;  // 止损下单类型，0市价， 1对手价
    double win_loss{0};
    QChar working_period;   // 有效， 0 本次登录有效，  1本地永久有效
};

//
struct autoTimeControl
{
    bool is_day;
    QTime day_begin;
    QTime day_end;

    bool is_night;
    QTime night_begin;
    QTime night_end;

};



//网络包头
typedef struct TCP_Head
{
    quint16						wPacketSize;						//数据大小
    quint16						wMainCmdID;							//主命令码
    quint16						wSubCmdID;							//子命令码
} TCP_Head;


// 获取K线的结构体信息
struct GetKlineData{
    quint16 type;                               // 类型，0-画K线，1-现在到本地
    quint16 frequence;                          // K线频率
    quint16 begin_time;                         // 开始时间
    quint16 end_time;                           // 结束时间
    quint16 kline_size;                         // 获取K线个数
    char symbol[32];                            // 合约名称
    char exchange_id[16];                       // 交易所名称
};


////////////////////////////////子类结构///////////////////////////////

//内核命令
#define MDM_KN_COMMAND					0								//内核命令

#define SUB_KN_DETECT_SOCKET			1								//检测命令

//登录命令
#define MDM_GR_LOGON					1								//登录命令

#define SUB_GR_LOGON_USER				100								//用户登录

//登录命令
#define MDM_GR_KLINE					2								//获取K线

#define SUB_GR_KLINE_INFO				100								//基本信息





#endif // PUBLICSTRUCT_H
