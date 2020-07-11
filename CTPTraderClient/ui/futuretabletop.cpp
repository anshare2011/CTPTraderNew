#include "futuretabletop.h"
#include "ui_futuretabletop.h"

FutureTableTop::FutureTableTop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FutureTableTop)
{
    ui->setupUi(this);
    this->setStyleSheet("color: darkCyan;font:14pt '宋体';");

    listView = new QListView(ui->chooseSymbol);
    model = new QStringListModel(ui->chooseSymbol);
    listView->setWindowFlags(Qt::ToolTip);

    connect(ui->chooseSymbol, SIGNAL(textChanged(const QString &)), this, SLOT(setCompleter(const QString &)));
    connect(listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));
}

FutureTableTop::~FutureTableTop()
{
    delete listView;
    delete ui;
}

void FutureTableTop::receiveQuoteInformation(std::shared_ptr<QuoteInfo> quote)
{
    if(ui->chooseSymbol->text() == quote->symbol)
    {
        // ui->symbol->setText(quote->symbol);
        ui->lastPrice->setText(QString::number(quote->lastPrice));

        ui->ask1Price->setText(QString::number(quote->askPrice1));
        ui->ask1Num->setText(QString::number(quote->askVolume1));
        ui->bid1Price->setText(QString::number(quote->bidPrice1));
        ui->bid1Num->setText(QString::number(quote->bidVolume1));

        ui->zhangdie->setText(QString::number(quote->lastPrice - quote->preSettlementPrice));
        ui->highPrice->setText(QString::number(quote->highPrice));
        ui->lowPrice->setText(QString::number(quote->lowPrice));
        ui->preSettlePrice->setText(QString::number(quote->preSettlementPrice));
        ui->time->setText(quote->time);
    }
}

double FutureTableTop::getLastPrice()
{
    return ui->lastPrice->text().toDouble();
}

void FutureTableTop::LineEditkeyPressEvent(QKeyEvent *e)
{
    qDebug()<<"FutureTableTop::LineEditkeyPressEvent";
    if (!listView->isHidden()) {
        int key = e->key();
        int count = listView->model()->rowCount();
        QModelIndex currentIndex = listView->currentIndex();

        if (Qt::Key_Down == key) {
            // 按向下方向键时，移动光标选中下一个完成列表中的项
            int row = currentIndex.row() + 1;
            if (row >= count) {
                row = 0;
            }
            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);

        } else if (Qt::Key_Up == key) {
            // 按向下方向键时，移动光标选中上一个完成列表中的项
            int row = currentIndex.row() - 1;
            if (row < 0) {
                row = count - 1;
            }
            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);

        } else if (Qt::Key_Escape == key) {
            // 按下Esc键时，隐藏完成列表
            listView->hide();
        } else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
            // 按下回车键时，使用完成列表中选中的项，并隐藏完成列表
            if (currentIndex.isValid()) {
                QString text = listView->currentIndex().data().toString();
                ui->chooseSymbol->setText(text);
            }
            listView->hide();
        } else {
            // 其他情况，隐藏完成列表，并使用QLineEdit的键盘按下事件
            listView->hide();
            // ui->lineEdit->keyPressEvent(e);
            // QLineEdit::keyPressEvent(e);
        }
    } else {
        // QLineEdit::keyPressEvent(e);
    }
}

//void FutureTableTop::on_lineEdit_textChanged(const QString &arg1)
//{

//}

void FutureTableTop::setCompleter(const QString &text)
{
    if (text.isEmpty()) {
        listView->hide();
        return;
    }

    QStringList sl;
    QStringList& option_list = m_me->getOptionList();
    foreach(QString word, option_list) {
        if (word.contains(text)) {
            sl << word;
        }
    }

    model->setStringList(sl);
    listView->setModel(model);

    if (model->rowCount() == 0) {
        return;
    }

    // Position the text edit
    listView->setMinimumWidth(ui->chooseSymbol->width());
    listView->setMaximumWidth(ui->chooseSymbol->width());

    QPoint p = ui->chooseSymbol->pos();
    int height = ui->chooseSymbol->height();
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + height;
    listView->move(x, y);
    listView->show();

    if(m_me->IsOption(text))
    {
        ui->symbol->setText(text);
        m_me->meSubscribeOption(text);

        emit sendSelectedOption(text);
    }
}

void FutureTableTop::completeText(const QModelIndex &index)
{
    QString text = index.data().toString();
    ui->chooseSymbol->setText(text);
    listView->hide();

    if(m_me->IsOption(text))
    {
        ui->symbol->setText(text);
        m_me->meSubscribeOption(text);

        emit sendSelectedOption(text);
    }
}
