#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QLineEdit>
#include <QThread>
#include "mainengine.h"
#include "tab_order_trade.h"
#include "tab_account_position.h"

#include "klinechart.h"
#include "ui/login.h"

#include "ui/searchmainwindow.h"
#include "ui/sendorder.h"
#include "ui/globalsetting.h"
#include "ui/searchinstrumentinformation.h"
#include "ui/strategymanagement.h"
#include "ui/backtest.h"
#include "ui/setquicktrade.h"
#include "ui/addipport.h"
#include "getdatafromserver.h"
#include "appsettings.h"

extern MainEngine* m_me;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init();
    // 初始化网络
    void initNetWork();
    void setBackground();
    void setMenu();
    void setTool();
    void setStatus();
    void registerMetaType();
    void addConnect();
    void mainwindowConnectToMainengine();
    void mainwindowConnectToDataengine();
    void mainengineConnectToMainwindow();
    void dataengineConnectToMainwindow();

    void menuLoginAddConnect(QMenu *menu);
    void menuSimulateAddConnect(QMenu *menu);
    void menuBackTestingAddConnect(QMenu *menu);
    void menuStrategyAddConnect(QMenu *menu);
    void menuHelpAddConnect(QMenu *menu);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

private slots:
    void receiveInformation(QString theme, QString content);



private:
    Ui::MainWindow *ui;

    KlineChart* m_kline = nullptr;                                      // K线表
    Tab_Account_Position *m_account_position = nullptr;                 // 账户持仓表
    Tab_Order_Trade *m_currentorder_historytrade = nullptr;             // 委托成交表

    Login *m_login = nullptr;                                           // 登录框
    SearchMainwindow *m_searchmian = nullptr;                           // 搜索合约
    SendOrder *m_sendorder = nullptr;                                   // 下单                                       // 开启rpc
    GlobalSetting *m_globalsetting = nullptr;                           // 配置
    SearchInstrumentInformation *m_serachinstrument = nullptr;          // 查询合约信息
    StrategyManageMent *m_strategy_manage = nullptr;                    // 策略管理
    BackTest *m_backtest = nullptr;                                     // 回测管理
    SetQuickTrade *m_quicktrade = nullptr;                              // 设置快捷交易
    GetDataFromServer *m_getdata = nullptr;                             // 网络
    AddIpPort *m_ipport = nullptr;                                      // 添加ip port


    QPoint right_down_point;   // 右下角的位置
    QSplitter *splitterMain = nullptr;   //新建主分割窗口，水平分割
    QSplitter *splitterUp = nullptr;
    QSplitter *splitterDown = nullptr;
};

#endif // MAINWINDOW_H
