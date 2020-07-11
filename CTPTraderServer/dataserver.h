#ifndef DATASERVER_H
#define DATASERVER_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QList>
#include <QMap>
#include "publicstruct.h"
#include "mydatabase.h"

// 提供数据查询服务
// 参考 https://blog.csdn.net/sinat_21107433/article/details/80150575

class DataServer : public QObject
{
    Q_OBJECT
public:
    explicit DataServer(QObject *parent = 0);
    // 开启服务器
    void start();

public slots:
    void receiveInstrumentInformation(InstrumentInfo);

signals:

public slots:
    void acceptConnection();
    // 接收数据
    void dataReceived();

private:
    QTcpServer *m_tcpServer {nullptr};
    QTcpSocket *m_tcpServerConnection {nullptr};

    qint64 m_bytesReceived;  //接收的字节数
    qint64 m_fileNameSize;   //文件名字的字节数
    qint64 perDataSize;      //每次发送的大小
    QString m_fileName;

    QByteArray m_inBlock;     //数据缓冲区
    QByteArray m_outBlock;

    struct TransferData{
        uint frequence;
        uint begin_time;
        uint end_time;
        char symbol[16];
        char exchange_id[16];
    };

    struct DataInfo{
        int size;
        char symbol[16];
    };

    QMap<QString, QString> symbol_exchange;
};

#endif // DATASERVER_H
