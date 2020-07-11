#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QList>
#include <QThread>
#include "tcpsocket.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QString ip, int port, QTcpServer *parent = 0);
    bool begin_listen();

    ~TcpServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:
    void sendNewConnectAddress(QString);
    void sendCloseConnectAddress(QString);

public slots:
    void SocketDisconnect(QString address);

private:
    QList<TcpSocket*> m_socketList;

    QString ip;
    int port;
};

#endif // TCPSERVER_H
