#ifndef SEARCHINSTRUMENTINFORMATION_H
#define SEARCHINSTRUMENTINFORMATION_H

#include <QWidget>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include "publicstruct.h"
#include "mainengine.h"

extern MainEngine *m_me;

namespace Ui {
class SearchInstrumentInformation;
}

class SearchInstrumentInformation : public QWidget
{
    Q_OBJECT

public:
    explicit SearchInstrumentInformation(QWidget *parent = 0);
    ~SearchInstrumentInformation();

protected:
    void showEvent(QShowEvent *event);

private slots:
    void on_lineEdit_returnPressed();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_Subscribe_instrument_clicked();

    void doubleClicked(int,int);

private:
    void search();
    // 初始表格展示所有
    void showAllInstrument();

private:
    Ui::SearchInstrumentInformation *ui;
    QStringList headerName;
};

#endif // SEARCHINSTRUMENTINFORMATION_H
