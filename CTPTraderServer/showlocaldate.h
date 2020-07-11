#ifndef SHOWLOCALDATE_H
#define SHOWLOCALDATE_H

#include <QWidget>
#include <QFileDialog>
#include <QtSql>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QList>

namespace Ui {
class ShowLocalDate;
}

class ShowLocalDate : public QWidget
{
    Q_OBJECT

public:
    explicit ShowLocalDate(QWidget *parent = 0);
    ~ShowLocalDate();

private slots:
    void on_choseFile_clicked();

    void on_showFile_clicked();

    void checkself(QTreeWidgetItem* ,int);

private:
    Ui::ShowLocalDate *ui;
    QSqlDatabase db;
    QList<QTreeWidgetItem *> item_list;
};

#endif // SHOWLOCALDATE_H
