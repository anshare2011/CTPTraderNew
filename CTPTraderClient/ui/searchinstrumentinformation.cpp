#include "searchinstrumentinformation.h"
#include "ui_searchinstrumentinformation.h"

SearchInstrumentInformation::SearchInstrumentInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchInstrumentInformation)
{
    ui->setupUi(this);
    this->setStyleSheet("font:11pt ;background-color: rgb( 90, 90, 90);color: white;");    //字体大小颜色
    this->setWindowTitle(QString::fromLocal8Bit("查询合约"));
    // 背景文字
    ui->lineEdit->setPlaceholderText(QString::fromLocal8Bit("输入相关信息可查询相关合约信息,双击所在行可订阅相关合约"));
    ui->lineEdit->setStyleSheet("color: white;");

    headerName.empty();
    headerName.append(QString::fromLocal8Bit("合约代码"));
    headerName.append(QString::fromLocal8Bit("名称"));
    headerName.append(QString::fromLocal8Bit("交易所"));
    headerName.append(QString::fromLocal8Bit("最后交割日"));
    headerName.append(QString::fromLocal8Bit("保证金率"));
    headerName.append(QString::fromLocal8Bit("合约乘数"));
    headerName.append(QString::fromLocal8Bit("开仓手续费"));
    headerName.append(QString::fromLocal8Bit("平仓手续费"));
    headerName.append(QString::fromLocal8Bit("平今手续费"));
    headerName.append(QString::fromLocal8Bit("最小变动单位"));
    headerName.append(QString::fromLocal8Bit("是否可以交易"));
    headerName.append(QString::fromLocal8Bit("持仓量"));
    headerName.append(QString::fromLocal8Bit("成交量"));
    ui->tableWidget->setStyleSheet("QHeaderView::section{background-color:rgb( 69, 137, 148); color: white;};");
    ui->tableWidget->setColumnCount(headerName.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerName);
    //Table->setShowGrid(false);                                 //隐藏表格线
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);//Stretch ResizeToContents);  //将表头显示在表内
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);             //选中一行
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);              //表格不可编辑
    ui->tableWidget->verticalHeader()->setHidden(true);                               // 隐藏行号

    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(doubleClicked(int,int)));
}

SearchInstrumentInformation::~SearchInstrumentInformation()
{
    delete ui;
}

void SearchInstrumentInformation::showEvent(QShowEvent *event)
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
    //清空表格
    for(int i=ui->tableWidget->rowCount();i>=0;i--)
    {
        ui->tableWidget->removeRow(i);
    }
    this->showAllInstrument();

    //将该事件传给父类处理
    QWidget::showEvent(event);
}

// 代码重复
void SearchInstrumentInformation::on_lineEdit_returnPressed()
{
    QString instrument = ui->lineEdit->text();
    QMap <QString, InstrumentInfo> &allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size() <1)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("本地合约数据为空"),QString::fromLocal8Bit("请确认TD接口已成功连接."),QMessageBox::Ok);
        return;
    }
    QMap <QString, InstrumentInfo>::iterator it;
    for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
    {
        if(it->symbol.toUpper() == instrument.toUpper())
        {
            m_me->meSubscribeInstrument(it->symbol);
        }
    }
}

void SearchInstrumentInformation::on_lineEdit_textChanged(const QString &arg1)
{
    //清空表格
    for(int i=ui->tableWidget->rowCount();i>=0;i--)
    {
        ui->tableWidget->removeRow(i);
    }
    QMap <QString, InstrumentInfo> &allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size() <1)
        return;
    if(arg1.size() <1)
    {
        this->showAllInstrument();
    }
    else
    {
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
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem(it->exchangeId));
                ui->tableWidget->setItem(row, 3, new QTableWidgetItem(it->deadline.toString("yyyy.MM.dd")));
                ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(it->marginRate)));
                ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(it->multiplier)));
                ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(it->openCommission)));
                ui->tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(it->closeCommission)));
                ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(it->closeTodayCommission)));
                ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(it->minimumUnit)));
                ui->tableWidget->setItem(row, 10, new QTableWidgetItem(QString::number(it->tradable)));
                ui->tableWidget->setItem(row, 11, new QTableWidgetItem(QString::number(it->openInterest)));
                ui->tableWidget->setItem(row, 12, new QTableWidgetItem(QString::number(it->tempVolume)));
            }
        }
    }
}

void SearchInstrumentInformation::on_Subscribe_instrument_clicked()
{
    QString instrument = ui->lineEdit->text();
    QMap <QString, InstrumentInfo> &allInstruments = m_me->getInstrumentInfo();
    if(allInstruments.size() <1)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("本地合约数据为空"),QString::fromLocal8Bit("请确认TD接口已成功连接."),QMessageBox::Ok);
        return;
    }
    QMap <QString, InstrumentInfo>::iterator it;
    for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
    {
        if(it->symbol.toUpper() == instrument.toUpper())
        {
            m_me->meSubscribeInstrument(it->symbol);
        }
    }
}

void SearchInstrumentInformation::doubleClicked(int row, int column)
{
    QTableWidgetItem * item1 = ui->tableWidget->item(row, 0);
    QString instrument = item1->text();
    m_me->meSubscribeInstrument(instrument);
}

void SearchInstrumentInformation::showAllInstrument()
{
    QMap <QString, InstrumentInfo> &allInstruments = m_me->getInstrumentInfo();
    QMap <QString, InstrumentInfo>::iterator it;
    for(it=allInstruments.begin(); it!=allInstruments.end(); ++it)
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it->symbol));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(it->name));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(it->exchangeId));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(it->deadline.toString("yyyy.MM.dd")));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(it->marginRate)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(it->multiplier)));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(it->openCommission)));
        ui->tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(it->closeCommission)));
        ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(it->closeTodayCommission)));
        ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(it->minimumUnit)));
        ui->tableWidget->setItem(row, 10, new QTableWidgetItem(QString::number(it->tradable)));
        ui->tableWidget->setItem(row, 11, new QTableWidgetItem(QString::number(it->openInterest)));
        ui->tableWidget->setItem(row, 12, new QTableWidgetItem(QString::number(it->tempVolume)));
    }
}
