#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->init();
    this->setMenu();
    this->addConnect();
}

MainWindow::~MainWindow()
{
    delete m_login;
    delete ui;

    m_control->stop();
    delete m_control;
}

void MainWindow::init()
{
    this->setWindowTitle("CTPTraderServer");
    m_login = new Login();
    m_showlocal = new ShowLocalDate();
    m_timecontrol = new TimeControl();
    m_me = new MainEngine();

    m_control = new ControlThread();
    m_control->start();
}

void MainWindow::setMenu()
{
    QMenuBar *mb = ui->menubar;//ui->menuBar;//this->menuBar();

    QMenu *menuLogin = mb->addMenu(QString::fromLocal8Bit("登录(L)"));
    this->menuLoginAddConnect(menuLogin);
}

void MainWindow::menuLoginAddConnect(QMenu *menu)
{
    QAction *actionLogin = menu->addAction(QString::fromLocal8Bit("登录"));
    connect(actionLogin,&QAction::triggered,[=](){
        m_login->show();
    });

    QAction *actionShowLocal = menu->addAction(QString::fromLocal8Bit("查看本地数据库"));
    connect(actionShowLocal,&QAction::triggered,[=](){
        m_showlocal->show();
    });

    QAction *actiontime = menu->addAction(QString::fromLocal8Bit("时间设置"));
    connect(actiontime, &QAction::triggered, [=](){
        m_timecontrol->show();
    });
}

void MainWindow::addConnect()
{
    connect(m_login, &Login::sendLoginInformation, [=](std::shared_ptr<LoginInformation> login){
        m_me->meLogin(login);
    });

    connect(m_control, SIGNAL(mdtdIsLogin(bool)), this, SLOT(controlIsLogin(bool)));
}

void MainWindow::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    int port = ui->spinBox->value();
    m_server = new TcpServer(ip, port);

    connect(m_server, SIGNAL(sendNewConnectAddress(QString)), this, SLOT(receiveNewAddress(QString)));
    connect(m_server, SIGNAL(sendCloseConnectAddress(QString)), this, SLOT(receiveCloseAddress(QString)));

    bool ret = m_server->begin_listen();
    if(ret)
        ui->pushButton->setText(QString::fromLocal8Bit("已开启监听"));
    else
        QMessageBox::information(this, QString::fromLocal8Bit("监听失败"), QString::fromLocal8Bit("监听失败，请更换端口"), QMessageBox::Ok);
}

void MainWindow::receiveNewAddress(QString)
{

}

void MainWindow::receiveCloseAddress(QString)
{

}

void MainWindow::controlIsLogin(bool is_login)
{
    m_me->controlIsLogin(is_login);
}
