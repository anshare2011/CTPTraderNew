#ifndef SENDORDER_H
#define SENDORDER_H

#include <QWidget>
#include "publicstruct.h"
#include "mainengine.h"

extern MainEngine* m_me;

// 下单板

namespace Ui {
class SendOrder;
}

class SendOrder : public QWidget
{
    Q_OBJECT

public:
    explicit SendOrder(QWidget *parent = nullptr);
    ~SendOrder();

public slots:
    void receiveQuoteInformation(std::shared_ptr<QuoteInfo>);

private slots:
    void on_pushButton_clicked();

private:
    Ui::SendOrder *ui;

};

#endif // SENDORDER_H
