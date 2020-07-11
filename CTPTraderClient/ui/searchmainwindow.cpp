#include "searchmainwindow.h"
#include "ui_searchmainwindow.h"

SearchMainwindow::SearchMainwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchMainwindow)
{
    ui->setupUi(this);
    // 隐藏边框
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setMinimumWidth(300);
    this->setMinimumHeight (300);
    this->setMaximumWidth (300);
    this->setMaximumHeight (300);

    ui->lineEdit->setClearButtonEnabled(true);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//Stretch ResizeToContents);  //将表头显示在表内
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);             //选中一行
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);              //表格不可编辑
    ui->tableWidget->verticalHeader()->setHidden(true);

    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(doubleClicked(int,int)));
}

SearchMainwindow::~SearchMainwindow()
{
    delete ui;
}

void SearchMainwindow::showAtRightDownPoint(QPoint &p)
{
    this->setGeometry(p.x()- this->width(), p.y() - this->height(), this->width(), this->height());
    this->show();
}

void SearchMainwindow::showEvent(QShowEvent *event)
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
    ui->tableWidget->clearFocus();
    //清空表格
    for(int i=ui->tableWidget->rowCount();i>=0;i--)
    {
        ui->tableWidget->removeRow(i);
    }
    this->showAllInstrument();
}

void SearchMainwindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<< "SearchMainwindow::keyPressEvent"<< event->key();
    switch (event->key()) {
    case Qt::Key_Down:
        if(ui->tableWidget->rowCount() >0)
        {
            ui->lineEdit->clearFocus();
            ui->tableWidget->setFocus();
            ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
            int row = ui->tableWidget->currentRow();
            if(0 == row)
                ui->tableWidget->selectRow(0);
        }
        break;
    case Qt::Key_Up:
        if(ui->tableWidget->rowCount() >0)
        {
            int row = ui->tableWidget->currentRow();
            if(0 == row)
            {
                ui->lineEdit->setFocus();
                ui->tableWidget->clearFocus();
            }
        }
        break;
    case Qt::Key_Return:
        // 只处理 tableWidget 的回车事件
        if(ui->tableWidget->rowCount() >0 && ui->tableWidget->hasFocus())
        {
            // 代码重复
            int row = ui->tableWidget->currentRow();
            QTableWidgetItem * item1 = new QTableWidgetItem;
            item1 = ui->tableWidget->item(row, 0);
            QString instrument = item1->text();
            m_me->meSubscribeInstrument(instrument);
            delete item1;
            this->close();
        }
        break;
    default:
        break;
    }
}

// 处理搜索框的回车事件
void SearchMainwindow::on_lineEdit_returnPressed()
{
//    qDebug()<< "SearchMainwindow::on_lineEdit_returnPressed";
    QString symbol = ui->lineEdit->text();
    QMap <QString, InstrumentInfo> allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size() <1)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("本地合约数据为空"),QString::fromLocal8Bit("请确认TD接口已成功连接."),QMessageBox::Ok);
        return;
    }
    QMap <QString, InstrumentInfo>::iterator it;
    for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
    {
        // 搜索框输入合约不用区分大小写
        if(it->symbol.toUpper() == symbol.toUpper())
        {
            m_me->meSubscribeInstrument(it->symbol);
        }
    }
    this->close();
}

void SearchMainwindow::on_lineEdit_textChanged(const QString &arg1)
{
    //清空表格
    for(int i=ui->tableWidget->rowCount();i>=0;i--)
    {
        ui->tableWidget->removeRow(i);
    }
    QMap <QString, InstrumentInfo> allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size() <1)
        return;

    QMap <QString, InstrumentInfo>::iterator it;
    for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
    {
        if(it->symbol.toUpper().contains(arg1.toUpper()) || it->name.toUpper().contains(arg1.toUpper()) || it->exchangeId.toUpper().contains(arg1.toUpper()))
        {
            //将持仓写入表中
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it->symbol));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(it->name));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(it->openInterest)));
        }
    }
}

void SearchMainwindow::doubleClicked(int row, int column)
{
//    qDebug()<< "SearchMainwindow::doubleClicked"<< row;
    QTableWidgetItem * item1 = new QTableWidgetItem;
    item1 = ui->tableWidget->item(row, 0);
    QString instrument = item1->text();
    m_me->meSubscribeInstrument(instrument);
}

void SearchMainwindow::showAllInstrument()
{
    QMap <QString, InstrumentInfo> allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size()>0)
    {
        QMap <QString, InstrumentInfo>::iterator it;
        for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
        {
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it->symbol));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(it->name));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(it->openInterest)));
        }
    }
}
