#ifndef SEARCHMAINWINDOW_H
#define SEARCHMAINWINDOW_H

#include <QWidget>
#include <QPoint>
#include <QShowEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "mainengine.h"
#include "publicstruct.h"

// 回车，搜索-订阅合约

extern MainEngine* m_me;

namespace Ui {
class SearchMainwindow;
}

class SearchMainwindow : public QWidget
{
    Q_OBJECT

public:
    explicit SearchMainwindow(QWidget *parent = 0);
    ~SearchMainwindow();
    void showAtRightDownPoint(QPoint &p);

protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_lineEdit_returnPressed();

    void on_lineEdit_textChanged(const QString &arg1);

    void doubleClicked(int,int);

private:
    // 初始表格展示所有
    void showAllInstrument();

private:
    Ui::SearchMainwindow *ui;
};

#endif // SEARCHMAINWINDOW_H
