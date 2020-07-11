#include "tcpserver.h"

TcpServer::TcpServer(QString ip, int port, QTcpServer *parent)
{
    this->ip = ip;
    this->port = port;
}

bool TcpServer::begin_listen()
{
    bool bsucc;
    if (this->ip.size()< 1)
    {
        bsucc = this->listen(QHostAddress::AnyIPv4, port);
    }
    else
    {
        bsucc = this->listen(QHostAddress::LocalHost, port);
    }
    qDebug() << QString::fromLocal8Bit("服务器监听（true表示成功）") << bsucc;
    return bsucc;

}

TcpServer::~TcpServer()
{
    this->close();
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << QString::fromLocal8Bit("服务器监听到新的连接");

    TcpSocket *socket = new TcpSocket(socketDescriptor);
    QThread *thread = new QThread();

    // 连接信息
    QString address = socket->peerAddress().toString();
    QString port = QString::number(socket->peerPort());

    //连接通知
    emit sendNewConnectAddress(address);

    connect(socket, SIGNAL(readyRead()), socket, SLOT(ReadAndParseData()));             // 会移进线程里
    connect(socket, SIGNAL(socketDisConnect(QString)), this, SLOT(SocketDisconnect(QString)));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), thread, SLOT(quit()));

    //开启线程
    socket->moveToThread(thread);
    thread->start();

    //通知连接
    emit newConnection();

}

void TcpServer::SocketDisconnect(QString address)
{
    //断开通知
    emit sendCloseConnectAddress(address);
}

