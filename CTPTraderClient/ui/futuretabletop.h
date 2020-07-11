#ifndef FUTURETABLETOP_H
#define FUTURETABLETOP_H

#include <QWidget>
#include <QLineEdit>
#include <QStringList>
#include <QListView>
#include <QStringListModel>
#include <QKeyEvent>
#include <QDebug>
#include "mainengine.h"

extern MainEngine* m_me;

// 期权页上部分控件
// 参考 https://www.cnblogs.com/findumars/p/4982795.html

namespace Ui {
class FutureTableTop;
}

class FutureTableTop : public QWidget
{
    Q_OBJECT

public:
    explicit FutureTableTop(QWidget *parent = nullptr);
    ~FutureTableTop();

    void receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote);

    double getLastPrice();

signals:
    void sendSelectedOption(QString);

private slots:
    // void on_lineEdit_textChanged(const QString &arg1);

    void LineEditkeyPressEvent(QKeyEvent *e);

    void setCompleter(const QString &text);             // 动态的显示完成列表
    void completeText(const QModelIndex &index);        // 点击完成列表中的项，使用此项自动完成输入的单词

private:
    Ui::FutureTableTop *ui;

    QListView *listView;                                // 完成列表
    QStringListModel *model;                            // 完成列表的model
};

#endif // FUTURETABLETOP_H
