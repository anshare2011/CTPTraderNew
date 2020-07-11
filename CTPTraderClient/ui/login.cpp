#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("欢迎登录"));
    this->setFocusPolicy(Qt::StrongFocus);
    this->init();
}

Login::~Login()
{
    delete ui;
}

void Login::init()
{
    // 密码格式，保存账户
    ui->Password_Edit->setEchoMode(QLineEdit::Password);
    // 密码框设置只能输入字母与数字
    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator1 = new QRegExpValidator(regx, ui->Password_Edit );
    ui->Password_Edit->setValidator(validator1);
    ui->SaveAccount->setChecked(true);
    ui->SavePassword->setChecked(true);

    QFileInfo fileInfo(CONFIG_FILE_NAME);
    if(fileInfo.exists() == true)
        this->readConfigurationFile();
}

void Login::on_pushButton_login_clicked()
{
    std::shared_ptr<LoginInformation> login_info(new LoginInformation);
    login_info->mdAddress = ui->Md_Edit->text();
    login_info->tdAddress = ui->Td_Edit->text();
    login_info->brokerID = ui->Broker_Edit->text();
    login_info->userName = ui->Account_Edit->text();
    login_info->password = ui->Password_Edit->text();

    login_info->AppID = ui->AppID_Edit->text();
    login_info->authorizationCode = ui->Authorization_Edit->text();
    login_info->productionInfo = ui->ProductionInfo_Edit->text();
    if(0== login_info->tdAddress.size() || 0== login_info->brokerID.size() ||
            0==login_info->userName.size()  || 0 ==login_info->password.size() )
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("登录错误"),
                             QString::fromLocal8Bit("登录信息为空，请重新确认!"), QMessageBox::Ok);
        return;
    }
    // 发送登录信号
    emit sendLoginInformation(login_info);

    if(ui->SaveAccount->isChecked())
        this->saveLoginInformationToFile();
    this->close();
}

void Login::on_pushButton_simnow_registered_clicked()
{
    QUrl url(QString("http://www.simnow.com.cn/"));
    bool is_open = QDesktopServices::openUrl(url);
    qDebug()<< QString::fromLocal8Bit("打开simnow注册按钮")<< is_open;
}

void Login::readConfigurationFile()
{
    //读配置文件
    QSettings configIniRead(CONFIG_FILE_NAME, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString md = configIniRead.value("/account/mdAddress").toString();
    ui->Md_Edit->setText(md);
    QString td = configIniRead.value("account/tdAddress").toString();
    ui->Td_Edit->setText(td);
    QString br = configIniRead.value("account/broker").toString();
    ui->Broker_Edit->setText(br);
    QString ac = configIniRead.value("account/account").toString();
    ui->Account_Edit->setText(ac);
    QString ps = configIniRead.value("account/password").toString();
    if (0 != ps.size()){
        QString ps1 = this->decryption(ps);
        ui->Password_Edit->setText(ps1);
    }
    QString appid = configIniRead.value("account/AppID").toString();
    ui->AppID_Edit->setText(appid);
    QString auth = configIniRead.value("account/AuthCode").toString();
    ui->Authorization_Edit->setText(auth);
    QString info = configIniRead.value("account/AppInfo").toString();
    ui->ProductionInfo_Edit->setText(info);
}

void Login::saveLoginInformationToFile()
{
    //创建 目录
    const QString path = "config/";
    QDir temp;
    if (!temp.exists(path))
    {
        temp.mkpath(path);
    }

    //QSettings configIniWrite(configurationFilePath, QSettings::IniFormat);
    QSettings configIniWrite(CONFIG_FILE_NAME, QSettings::IniFormat);

    //向ini文件中写入内容,setValue函数的两个参数是键值对
    //向ini文件的第一个节写入内容,account节下的第一个参数
    configIniWrite.setValue("account/mdAddress", ui->Md_Edit->text());
    configIniWrite.setValue("account/tdAddress", ui->Td_Edit->text());
    configIniWrite.setValue("account/broker", ui->Broker_Edit->text());
    configIniWrite.setValue("account/account", ui->Account_Edit->text());

    if(ui->SavePassword->isChecked()){
        QString save_password = this->encryption(ui->Password_Edit->text());
        configIniWrite.setValue("account/password", save_password);
    }
    else
        configIniWrite.setValue("account/password", "");
    configIniWrite.setValue("account/AppID", ui->AppID_Edit->text());
    configIniWrite.setValue("account/AuthCode", ui->Authorization_Edit->text());
    configIniWrite.setValue("account/AppInfo", ui->ProductionInfo_Edit->text());


    appSettingsInstance->setSetting("account1/mdAddress", ui->Md_Edit->text());
    appSettingsInstance->setSetting("account1/tdAddress", ui->Td_Edit->text());
    appSettingsInstance->setSetting("account1/broker", ui->Broker_Edit->text());
    appSettingsInstance->setSetting("account1/account", ui->Account_Edit->text());
    appSettingsInstance->setSetting("account1/AppID", ui->AppID_Edit->text());
    appSettingsInstance->setSetting("account1/AuthCode", ui->Authorization_Edit->text());
    appSettingsInstance->setSetting("account1/AppInfo", ui->ProductionInfo_Edit->text());
}

QString Login::encryption(QString str)
{
    QByteArray baPw = str.toUtf8();
    for (int i = 0; i < baPw.size(); i++)
    {
        int change_value = (i % 2) *i + (i % 3 - 2)*i + 1;
        if (i % 2 == 0)
            baPw[i] = baPw[i] + change_value;
        else
            baPw[i] = baPw[i] - change_value;
    }
    str = baPw;
    return str;
}

QString Login::decryption(QString str)
{
    QByteArray baPw1 = str.toUtf8();
    for (int i = 0; i < baPw1.size(); i++)
    {
        int change_value = (i % 2) *i + (i % 3 - 2)*i + 1;
        if (i % 2 == 0)
            baPw1[i] = baPw1[i] - change_value;
        else
            baPw1[i] = baPw1[i] + change_value;
    }
    str = baPw1;
    return str;
}

void Login::on_comboBox_currentIndexChanged(int index)
{
    if(index == 1)
    {
        ui->Md_Edit->setText("tcp://180.168.146.187:10110");
        ui->Td_Edit->setText("tcp://180.168.146.187:10100");
    }
    else if(index == 2)
    {
        ui->Md_Edit->setText("tcp://180.168.146.187:10111");
        ui->Td_Edit->setText("tcp://180.168.146.187:10101");
    }
    else if(index == 3)
    {
        ui->Md_Edit->setText("tcp://218.202.237.33:10112");
        ui->Td_Edit->setText("tcp://218.202.237.33:10102");
    }
    else if(index == 4)
    {
        ui->Md_Edit->setText("tcp://180.168.146.187:10131");
        ui->Td_Edit->setText("tcp://180.168.146.187:10130");
    }

    else if(index == 5)
    {
        ui->Md_Edit->setText("tcp://116.247.114.93:33213");
        ui->Td_Edit->setText("tcp://116.247.114.93:33205");
        ui->Broker_Edit->setText("0052");
    }
    if(index < 5){
        ui->AppID_Edit->setText("simnow_client_test");
        ui->Authorization_Edit->setText("0000000000000000");
    }
}
