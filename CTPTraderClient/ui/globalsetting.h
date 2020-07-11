#ifndef GLOBALSETTING_H
#define GLOBALSETTING_H

/*
 * GlobalSetting 全局配置界面
 * 1.   启动读取配置文件的信息
 * 2.   配置文件不存在默认生成
 */

#include <QWidget>
#include "appsettings.h"


namespace Ui {
class GlobalSetting;
}

class GlobalSetting : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalSetting(QWidget *parent = nullptr);
    ~GlobalSetting();

    void readConfig();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_pushButton_clicked();

private:
    Ui::GlobalSetting *ui;
};

#endif // GLOBALSETTING_H
