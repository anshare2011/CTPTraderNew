#include "dataserver.h"
#include <QtNetwork>

DataServer::DataServer(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer,SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void DataServer::start()
{
    int port = 12345;
    if(!m_tcpServer->listen(QHostAddress::LocalHost, port)){      // QHostAddress::Any
        qDebug()<< "DataServer::start listen failed"<< m_tcpServer->errorString();
        return;
    }
}

void DataServer::receiveInstrumentInformation(InstrumentInfo info)
{
    qDebug()<< "DataServer::receiveInstrumentInformation";
    if(!info.is_end)
    {
        QString symbol = info.symbol;
        QString exchange = info.exchangeId;
        if(!symbol_exchange.contains(symbol))
        {
            symbol_exchange.insert(symbol, exchange);
        }
    }
}

void DataServer::acceptConnection()
{
    qDebug()<< "DataServer::acceptConnection";
    m_tcpServerConnection = m_tcpServer->nextPendingConnection();
    connect(m_tcpServerConnection,SIGNAL(readyRead()),this,SLOT(dataReceived()));
}

void DataServer::dataReceived()
{
    qDebug()<<m_tcpServerConnection->bytesAvailable();
    if(m_tcpServerConnection->bytesAvailable() < sizeof(struct TransferData) )
    {
        return;
    }

    TransferData *msg = new TransferData();
    m_tcpServerConnection->read((char *)msg, sizeof(struct TransferData));

    uint frequence = msg->frequence;
    uint begin_time = msg->begin_time;
    uint end_time = msg->end_time;
    QString symbol_s = QString(QLatin1String(msg->symbol));
    QString exchange_id = QString(QLatin1String(msg->exchange_id));
    qDebug()<<"DataServer::dataReceived"<< symbol_s<< exchange_id<< sizeof(TransferData);


    QString table_name = "ag2006_min1";
    QString sql = QString("select * from %1 where timestamp > %2 and timestamp < %3 order by id desc limit 1000;")
            .arg(table_name, QString::number(begin_time), QString::number(end_time));

//    QString sql = QString("select * from ag2006_min1 order by id;");


    QList<TickData> data_list;
    MyDataBase my_bd("localhost", "SHFE.db");
    bool ret = my_bd.dbExec(sql, data_list);
    qDebug()<<"DataServer::my_bd.dbExec"<< sql<< ret<< data_list.size();

    // tick个数
    int count = data_list.size();
    int total_size = sizeof(DataInfo) + count* sizeof(TickData);
    qDebug()<<count<< total_size<< sizeof(TickData);
    QByteArray ba;
    ba.resize(total_size);

    // 写传输信息的结构体
    DataInfo datainfo;
    datainfo.size = total_size;
    qDebug()<< datainfo.size;
    strcpy(datainfo.symbol, msg->symbol);
    memcpy(ba.data() , &datainfo, sizeof(DataInfo));

    // 反向遍历
    for(int i=0; i<count; ++i){
        memcpy(ba.data() + sizeof(DataInfo) + i*sizeof(TickData), &(data_list.at(count -1 - i)), sizeof(TickData));
    }
    m_tcpServerConnection->write((char*)ba.data(), total_size);
}
