#ifndef TAB_ACCOUNT_POSITION_H
#define TAB_ACCOUNT_POSITION_H

#include <QWidget>
#include <QTableWidget>
#include <QStringList>
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include "publicstruct.h"
#include "mywidgets/mytabwidget.h"
#include "mainengine.h"

extern MainEngine* m_me;

class Tab_Account_Position : public MyTabWidget
{
    Q_OBJECT
public:
    explicit Tab_Account_Position(QWidget *parent = 0);
    ~Tab_Account_Position();

signals:

public slots:
    void receiveChiCangInformation(std::shared_ptr<PositionInfo>);
    void receiveAccountInformation(std::shared_ptr<AccountInfo>);
    void receiveLog(QString, QString, QString);

    // 平仓槽函数
    void slotOffsetPosition(const QPoint&);
    void offsetOnePosition();
    void reverseOnePosition();

private:
    void initAccountTable();
    void initPositionTable();
    void initLogTable();

    // 持仓表 右键平仓、反向开
    void positionTableOffset();

private:
    QTableWidget *table_account{nullptr};
    QStringList name_account;
    QTableWidget *table_poistion{nullptr};
    QStringList name_poistion;
    QTableWidget *table_log{nullptr};
    QStringList name_log;

    // 持仓表右键平仓
    QMenu *menu_position;
    QAction *action_offset_position;
    QAction *action_reverse_position;

    // 持仓表需要清空
    bool position_clear = false;
};

#endif // TAB_ACCOUNT_POSITION_H
