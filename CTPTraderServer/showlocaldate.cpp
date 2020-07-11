#include "showlocaldate.h"
#include "ui_showlocaldate.h"

ShowLocalDate::ShowLocalDate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowLocalDate)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Show Local Data"));
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)), this,SLOT(checkself(QTreeWidgetItem* ,int)));
    ui->treeWidget->setMaximumWidth(300);
}

ShowLocalDate::~ShowLocalDate()
{
    delete ui;
}

void ShowLocalDate::on_choseFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open a SQLite3 File"),QString(),"DB (*.db);;All files(*.*)");
    if (fileName.isEmpty())
        return;
    ui->lineEdit->setText(fileName);
}

void ShowLocalDate::on_showFile_clicked()
{
    QString filePath = ui->lineEdit->text();
    if (filePath.size()<1)
        return;
    // 路径获取文件名
    QStringList fileNameList = filePath.split("/");
    QString fileName = fileNameList.at(fileNameList.size()-1).split(".").at(0);

//    if(QSqlDatabase::contains("qsqlite")){   // qsqlite    qt_sql_default_connection
//        db = QSqlDatabase::database("qsqlite");
//    }else{
//        db = QSqlDatabase::addDatabase("QSQLITE", "qsqlite");
//    }

    quint32 value = QRandomGenerator::global()->generate();
    if(QSqlDatabase::contains(QString::number(value))){         // qsqlite    qt_sql_default_connection
        db = QSqlDatabase::database(QString::number(value));
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE", QString::number(value));
        db.setDatabaseName("DataBase/SHFE.db");
    }


    // db.setDatabaseName(filePath);
    if (!db.open())
        return;
    QSqlQuery query("select name from sqlite_master where type='table' order by name;", db);     // 查看db文件中的表名
    QStringList tableList;
    while (query.next()) {
        tableList << query.value(0).toString();
    }
    foreach(auto item, item_list)
    {
        item_list.removeOne(item);
        delete item;
    }

    ui->treeWidget->headerItem()->setText(0, fileName);
    for(int i =0; i<tableList.size();++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget, QStringList(tableList.at(i)));
        item_list.append(item);
    }
}

void ShowLocalDate::checkself(QTreeWidgetItem *tree, int i)
{
    QString tableName = tree->text(i);

    QSqlRelationalTableModel* model = new QSqlRelationalTableModel(NULL,db);
    model->setTable(tableName);
    model->select();
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择模式
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);   //使其不可编辑

}
