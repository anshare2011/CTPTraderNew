#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>
#include <QString>
#include <QSettings>    //写配置文件
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QRegExpValidator>
#include <memory>
#include "publicstruct.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    void init();

signals:
    void sendLoginInformation(std::shared_ptr<LoginInformation>);

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_simnow_registered_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    void readConfigurationFile();

    void saveLoginInformationToFile();

private:
    Ui::Login *ui;
    const QString configurationFilePath= "config/login_info.ini";
};

#endif // LOGIN_H
