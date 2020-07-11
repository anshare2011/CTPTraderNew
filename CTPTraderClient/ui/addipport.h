#ifndef ADDIPPORT_H
#define ADDIPPORT_H

/*
 * AddIpPort 添加服务器 ip port
 */


#include <QWidget>
#include <QMessageBox>
#include "getdatafromserver.h"

namespace Ui {
class AddIpPort;
}

class AddIpPort : public QWidget
{
    Q_OBJECT

public:
    explicit AddIpPort(QWidget *parent = nullptr);
    ~AddIpPort();

    bool isIpAddress(QString ip);

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddIpPort *ui;
};

#endif // ADDIPPORT_H
