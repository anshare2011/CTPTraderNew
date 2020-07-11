# CTPTraderNew

  一：项目背景
  
    期货ctp穿透之前的版本写了一个项目CTPTrader，现已无法使用（详见 https://github.com/huxingwu1/CTPTrader）。
    因此根据穿透ctp重新开启了这个项目，目前还在进一步完善更新中。
    
  二：项目介绍
  
    本项目下包含两个小项目：CTPTraderServer、CTPTraderClient.
    
    1.CTPTraderServer是行情收集程序，每天定时登录和退出，收集全市场期货程序，收集tick行情和将tick数据合成不同频率的K线数据保存本地数据库。
    CTPTraderServer可以开启监听服务，供CTPTraderClient请求获得历史行情K线。
  
    2.CTPTraderClient是期货交易客户端，目前手动交易已经基本ok，策略和回测还在添加，后期进一步完善。

    3.new目录下是已经编译好的程序，可直接解压运行。

  三：项目作者
  
    微信：17512015950
