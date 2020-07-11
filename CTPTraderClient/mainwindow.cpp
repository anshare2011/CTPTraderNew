#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);   // 参见 QWidget::keyPressEvent 说明文档，重写需要设置

    this->init();
    this->initNetWork();
    this->setBackground();
    this->setMenu();
    this->setTool();
    this->setStatus();
    this->registerMetaType();
    this->addConnect();
    this->mainwindowConnectToMainengine();
    this->mainwindowConnectToDataengine();
    this->dataengineConnectToMainwindow();
    this->mainengineConnectToMainwindow();
}

MainWindow::~MainWindow()
{
    delete m_login;
    delete m_searchmian;

    delete m_account_position;
    delete m_currentorder_historytrade;
    delete m_kline;

    delete ui;
}

void MainWindow::init()
{
    m_login = new Login();
    m_searchmian = new SearchMainwindow();


    m_account_position = new Tab_Account_Position(this);
    m_currentorder_historytrade = new Tab_Order_Trade(this);
    m_kline = new KlineChart(this);

    m_sendorder = new SendOrder();
    m_globalsetting = new GlobalSetting();
    m_serachinstrument = new SearchInstrumentInformation();
    m_strategy_manage = new StrategyManageMent();
    m_backtest = new BackTest();
    m_quicktrade = new SetQuickTrade();
    m_ipport = new AddIpPort();

    // https://blog.csdn.net/hl1hl/article/details/85244451
    // m_sendorder->setParent(this);//指定父窗口
    // m_sendorder->setWindowFlags(m_sendorder->windowFlags() | Qt::Dialog);


    QSplitter *up = new QSplitter(Qt::Horizontal);
    up->addWidget(m_kline);

    QSplitter *down = new QSplitter(Qt::Horizontal);
    QSplitter *down1 = new QSplitter(Qt::Horizontal, down);
    QSplitter *down2 = new QSplitter(Qt::Horizontal, down);
    down->setHandleWidth(1);
    down1->addWidget(m_currentorder_historytrade);
    down2->addWidget(m_account_position);


    splitterMain = new QSplitter(Qt::Vertical);   //新建主分割窗口，水平分割
    splitterUp = new QSplitter(Qt::Vertical, splitterMain);
    splitterDown = new QSplitter(Qt::Vertical, splitterMain);

    splitterMain->setHandleWidth(1);
    splitterUp->addWidget(up);
    splitterDown->addWidget(down);

    this->setCentralWidget(splitterMain);
}

void MainWindow::initNetWork()
{
    // 此处实例化单例
    // 将拿数据类加入线程，否则影响主线程接收行情。将耗时的工作写在DownloadDateFromServer的槽函数中
    // 此处添加信号与槽，所有拿的数据都在这里处理。

    m_getdata = GetDataFromServer::GetInstance();
    QThread *thread = new QThread;
    m_getdata->moveToThread(thread);
    thread->start();

    QString ip = appSettingsInstance->getSetting(SERVER_IP).toString();
    QString port = appSettingsInstance->getSetting(SERVER_PORT).toString();

    if(ip.size() <1 || port.size() <1)
    {
        m_ipport->setParent(this);//指定父窗口
        m_ipport->setWindowFlags(m_ipport->windowFlags()| Qt::Dialog);
        m_ipport->show();
    }
    else
    {
        quint16 port_quint16 = (quint16)port.toUInt();
        m_getdata->connectServer(ip, port_quint16);
    }
}

void MainWindow::setBackground()
{
    this->setWindowTitle(QString::fromLocal8Bit("CTPTraderClient"));
    this->setAutoFillBackground(true);
    this->setStyleSheet("background-color: rgb( 30, 30, 30);color: white; font:10pt");
}

void MainWindow::setMenu()
{
    QMenuBar *mb = ui->menuBar;  //this->menuBar();

    QMenu *menuLogin = mb->addMenu(QString::fromLocal8Bit("登录"));
    this->menuLoginAddConnect(menuLogin);

    QMenu *menuBackTesting = mb->addMenu(QString::fromLocal8Bit("回测"));
    this->menuBackTestingAddConnect(menuBackTesting);

    QMenu *menuStrategy = mb->addMenu(QString::fromLocal8Bit("策略"));
    this->menuStrategyAddConnect(menuStrategy);

    //    QMenu *menuSimulate = mb->addMenu(QString::fromLocal8Bit("模拟"));
    //    this->menuSimulateAddConnect(menuSimulate);

    QMenu *menuHelp = mb->addMenu(QString::fromLocal8Bit("帮助"));
    this->menuHelpAddConnect(menuHelp);

    QAction *actionSendorder = new QAction(QString::fromLocal8Bit("下单"));
    connect(actionSendorder,&QAction::triggered,[=](){
        // m_sendorder->setParent(this);//指定父窗口
        // m_sendorder->setWindowFlags(m_sendorder->windowFlags()| Qt::Dialog);
        m_sendorder->show();

    });
    mb->addAction(actionSendorder);

}

void MainWindow::setTool()
{
    // 隐藏工具栏
    QToolBar *toolbar = ui->mainToolBar;
    toolbar->setVisible(false);
}

void MainWindow::setStatus()
{

}

void MainWindow::registerMetaType()
{
    int id1 = qRegisterMetaType<std::shared_ptr<LoginInformation>>("std::shared_ptr<LoginInformation>");
    int id2 = qRegisterMetaType<std::shared_ptr<QuoteInfo>>("std::shared_ptr<QuoteInfo>");
    int id3 = qRegisterMetaType<std::shared_ptr<AccountInfo>>("std::shared_ptr<AccountInfo>");
    int id4 = qRegisterMetaType<InstrumentInfo>("InstrumentInfo");
    int id5 = qRegisterMetaType<std::shared_ptr<OrderInfo>>("std::shared_ptr<OrderInfo>");
    int id6 = qRegisterMetaType<std::shared_ptr<TradeInfo>>("std::shared_ptr<TradeInfo>");
    int id7 = qRegisterMetaType<QVector<BarData> >("QVector<BarData>");
    int id8 = qRegisterMetaType<BarData >("BarData");
    int id9 = qRegisterMetaType<OneSymbolKlineData* >("OneSymbolKlineData*");
    int id10 = qRegisterMetaType<std::shared_ptr<PositionInfo> >("std::shared_ptr<PositionInfo>");
    int id11 = qRegisterMetaType<stopWinLossOrder >("stopWinLossOrder");
    int id12 = qRegisterMetaType<OptionInfo >("OptionInfo");
    int id13 = qRegisterMetaType<CommissionRate >("CommissionRate");
    int id14 = qRegisterMetaType<KlineType >("KlineType");
}

void MainWindow::addConnect()
{
    // Login interface click login
    connect(m_login, &Login::sendLoginInformation, [=](std::shared_ptr<LoginInformation> login){
        m_me->meLogin(login);
    });

    connect(m_me, SIGNAL(sendPositionInformation(std::shared_ptr<PositionInfo>)),
            m_account_position, SLOT(receiveChiCangInformation(std::shared_ptr<PositionInfo>)));

    // 快捷下单信号
    connect(m_quicktrade, SIGNAL(sendQuickType(QuickType)), m_kline, SLOT(receiveQuickType(QuickType)));
}

void MainWindow::mainwindowConnectToMainengine()
{

}

void MainWindow::mainwindowConnectToDataengine()
{
    DataEngine* de = m_me->getDataenginePointer();
    connect(m_getdata, SIGNAL(sendKlineBarData(QString, KlineType, QVector<BarData>)),
            de, SLOT(receiveKlineBarData(QString, KlineType, QVector<BarData>)));
}

void MainWindow::mainengineConnectToMainwindow()
{
    connect(m_me, SIGNAL(sendAccountInformation(std::shared_ptr<AccountInfo>)),
            m_account_position, SLOT(receiveAccountInformation(std::shared_ptr<AccountInfo>)));

    connect(m_me, SIGNAL(sendOrderInformation(std::shared_ptr<OrderInfo>)),
            m_currentorder_historytrade, SLOT(receiveOrderInformation(std::shared_ptr<OrderInfo>)));

    connect(m_me, SIGNAL(sendTradeInformation(std::shared_ptr<TradeInfo>)),
            m_currentorder_historytrade, SLOT(receiveTradeInformation(std::shared_ptr<TradeInfo>)));

    connect(m_me, SIGNAL(sendLog(QString,QString,QString)),
            m_account_position, SLOT(receiveLog(QString,QString,QString)));

    connect(m_me, SIGNAL(sendCancelAllOrder()), m_currentorder_historytrade, SLOT(cancelAllOrder()));

    connect(m_me, SIGNAL(sendInformation(QString, QString)), this, SLOT(receiveInformation(QString, QString)));
}

void MainWindow::dataengineConnectToMainwindow()
{
    DataEngine *de = m_me->getDataenginePointer();

    // de 将行情数据传给 主界面 期货页
    connect(de, SIGNAL(sendQuoteInformation(std::shared_ptr<QuoteInfo>)),
            m_kline, SLOT(receiveQuoteInformation(std::shared_ptr<QuoteInfo>)));

    // de 将行情数据传给 下单板
    connect(de, SIGNAL(sendQuoteInformation(std::shared_ptr<QuoteInfo>)),
            m_sendorder, SLOT(receiveQuoteInformation(std::shared_ptr<QuoteInfo>)));

    // de传K线数据过来，页面K线刷新
    connect(de, SIGNAL(sendKlineInformation(QString,OneSymbolKlineData*)),
            m_kline, SLOT(receiveKlineInformation(QString,OneSymbolKlineData*)));

    // de传K线数据过来，策略管理进行处理
    connect(de, SIGNAL(sendKlineInformation(QString,OneSymbolKlineData*)),
            m_strategy_manage, SLOT(receiveKlineInformation(QString,OneSymbolKlineData*)));

}

void MainWindow::menuLoginAddConnect(QMenu *menu)
{
    QAction *actionLogin = menu->addAction(QString::fromLocal8Bit("登录"));
    connect(actionLogin,&QAction::triggered,[=](){
        m_login->show();
    });

}

void MainWindow::menuSimulateAddConnect(QMenu *menu)
{
    QAction *actionSetAccount = menu->addAction(QString::fromLocal8Bit("模拟账号"));
    connect(actionSetAccount,&QAction::triggered,[=](){


    });

    QAction *actionSimulate = menu->addAction(QString::fromLocal8Bit("推送模拟数据"));
    connect(actionSimulate,&QAction::triggered,[=](){

    });
}

void MainWindow::menuBackTestingAddConnect(QMenu *menu)
{
    QAction *actionBackTest = menu->addAction(QString::fromLocal8Bit("回测管理"));
    connect(actionBackTest, &QAction::triggered,[=](){
        m_backtest->show();
    });

    QAction *analysisResult = menu->addAction(QString::fromLocal8Bit("回测分析"));
    connect(analysisResult,&QAction::triggered,[=](){

    });
}

void MainWindow::menuStrategyAddConnect(QMenu *menu)
{
    QAction *actionStrategy = menu->addAction(QString::fromLocal8Bit("策略管理"));
    connect(actionStrategy, &QAction::triggered,[=](){
        m_strategy_manage->show();
    });
}

void MainWindow::menuHelpAddConnect(QMenu *menu)
{
    QAction *actionSetQuick = menu->addAction(QString::fromLocal8Bit("设置快捷交易"));
    connect(actionSetQuick, &QAction::triggered,[=](){
        m_quicktrade->show();
    });

    QAction *searchInstrument = menu->addAction(QString::fromLocal8Bit("查询合约"));
    connect(searchInstrument,&QAction::triggered,[=](){
        m_serachinstrument->show();
    });

    // m_globalsetting
    QAction *globalsetting = menu->addAction(QString::fromLocal8Bit("配置"));
    connect(globalsetting, &QAction::triggered,[=](){
        m_globalsetting->show();
    });
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x = event->globalX();
    int y =  event->globalY();
    if(x < right_down_point.x() && x > right_down_point.x() - m_searchmian->width() &&
            y > right_down_point.y() - m_searchmian->height() && y< right_down_point.y())
    {

    }
    else{
        if(!m_searchmian->isHidden())
        {
            m_searchmian->close();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->globalX();
    int y =  event->globalY();
    if(x> m_kline->width() || y > m_kline->height())
    {

    }
    else
    {

    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 组合按键 ctrl+L
    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_L))
    {
        m_login->show();
        return;
    }

    QString text_key = event->text();
    if(text_key.size() >0)
        m_quicktrade->receiveKeyPressEvent(event);

    switch (event->key()) {
    case Qt::Key_Escape:
        QMessageBox::StandardButton sb;
        sb = QMessageBox::question(this,QString::fromLocal8Bit("退出"),
                                   QString::fromLocal8Bit("请确认是否退出？"), QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
        if(sb == QMessageBox::Yes)
        {
            m_searchmian->close();
            this->close();
        }
        break;
    case Qt::Key_Return:
        m_searchmian->showAtRightDownPoint(right_down_point);
        break;
    case Qt::Key_Enter:
        m_searchmian->close();
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    right_down_point.setX(this->pos().x() + this->width());
    right_down_point.setY(this->pos().y() + this->height());
    m_searchmian->close();
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    right_down_point.setX(this->pos().x() + this->width());
    right_down_point.setY(this->pos().y() + this->height());
    m_searchmian->close();
}

void MainWindow::receiveInformation(QString theme, QString content)
{
    QMessageBox::information(this, theme, content, QMessageBox::Yes);
}

