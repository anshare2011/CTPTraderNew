#include "addipport.h"
#include "ui_addipport.h"

AddIpPort::AddIpPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddIpPort)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("配置文件服务器信息不存在，点击连接"));
}

AddIpPort::~AddIpPort()
{
    delete ui;
}

bool AddIpPort::isIpAddress(QString ip)
{
    int a = -1, b = -1, c = -1, d = -1;
    char s[200] = { 0 };
    sscanf_s(ip.toStdString().c_str(), "%d.%d.%d.%d%s", &a, &b, &c, &d, s);
    if (a > 255 || a < 0 || b>255 || b < 0 || c>255 || c < 0 || d>255 || d < 0)
    {
        return false;
    }
    if (s[0] != 0)
    {
        return false;
    }

    return true;
}

void AddIpPort::on_pushButton_clicked()
{
    QString ip = ui->ip->text();

    if(!isIpAddress(ip))
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("地址错误"),
                             QString::fromLocal8Bit("ip地址格式错误，正确地址格式如 127.0.0.1，请确认后c重新输入!"), QMessageBox::Ok);
        return;
    }
    quint16 port = (quint16)ui->port->value();
    GetDataFromServer *m_getdata = GetDataFromServer::GetInstance();
    m_getdata->connectServer(ip, port);

    this->close();
}
