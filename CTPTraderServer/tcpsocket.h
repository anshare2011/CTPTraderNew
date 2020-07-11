#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>
#include "define.h"
#include "publicstruct.h"


class QTcpSocket;

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(int socketdesc, QTcpSocket *parent = 0);
    ~TcpSocket();

signals:
    void socketDisConnect(QString);

public slots:
    void DetectSocket();
    void ReadAndParseData();
    void dealConnect(QString);
    void dealDisconnect();
    void SocketErr(QAbstractSocket::SocketError socketError);

    //发送数据
public:
    //发送数据
    bool StartSendData(quint16 wMainCmdID, quint16 wSubCmdID, const void *pData, quint16 wDataSize, bool bDelete);

    //移动数据
public:
    //移动数据
    void MoveMemoryCache(quint16 wBufferDataSize);

    //变量定义
private:
    int							m_wBufferSize;							//数据长度

    //接收对象
private:

    //定时检测
private:
    QTimer* m_detectTimer{ nullptr };

    QSqlDatabase m_db;


private:
    QString m_recvDataStr;
    QString address = "";
};

#endif // TCPSOCKET_H
