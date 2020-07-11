#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include "login.h"
#include "tcpserver.h"
#include "mainengine.h"
#include "showlocaldate.h"
#include "timecontrol.h"
#include "controlthread.h"

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

    void setMenu();

    void menuLoginAddConnect(QMenu *menu);

    void addConnect();

private slots:
    void on_pushButton_clicked();

    void receiveNewAddress(QString);
    void receiveCloseAddress(QString);

    // 控制登录退出
    void controlIsLogin(bool);

private:
    Ui::MainWindow *ui;
    Login *m_login {nullptr};
    ShowLocalDate *m_showlocal = nullptr;
    TimeControl *m_timecontrol = nullptr;

    ControlThread *m_control = nullptr;

    TcpServer *m_server{nullptr};
    MainEngine* m_me{nullptr};
};

#endif // MAINWINDOW_H
