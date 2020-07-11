#include "globalsetting.h"
#include "ui_globalsetting.h"

GlobalSetting::GlobalSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GlobalSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("配置"));

    this->readConfig();
}

GlobalSetting::~GlobalSetting()
{
    delete ui;
}

void GlobalSetting::readConfig()
{
    ui->ip->setText(appSettingsInstance->getSetting(SERVER_IP).toString());
    ui->port->setValue(appSettingsInstance->getSetting(SERVER_PORT).toString().toInt());

    if(appSettingsInstance->getSetting(EMAIL_IS_SEND).toInt() == 1 )
        ui->sendEmail->setChecked(true);
    else
        ui->notSendEmail->setChecked(true);
    ui->emailAccount->setText(appSettingsInstance->getSetting(EMAIL_ACCOUNT).toString());
    ui->emailPassword->setText(appSettingsInstance->getSetting(EMAIL_PASSWORD).toString());
}

void GlobalSetting::showEvent(QShowEvent *event)
{
    this->readConfig();
}

void GlobalSetting::on_pushButton_clicked()
{
//#define EMAIL_IS_SEND QString("email/isSend")
//#define EMAIL_ACCOUNT QString("email/account")
//#define EMAIL_PASSWORD QString("email/password")
    if(ui->sendEmail->isChecked())
        appSettingsInstance->setSetting(EMAIL_IS_SEND, 1);
    else
        appSettingsInstance->setSetting(EMAIL_IS_SEND, 0);

    appSettingsInstance->setSetting(EMAIL_ACCOUNT, ui->emailAccount->text());
    appSettingsInstance->setSetting(EMAIL_PASSWORD, ui->emailPassword->text());
}
