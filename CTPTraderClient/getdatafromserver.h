#ifndef GETDATAFROMSERVER_H
#define GETDATAFROMSERVER_H

#include <QObject>
#include <QDebug>
#include <QAbstractSocket>
#include <QDateTime>
#include <QFileDialog>
#include "publicstruct.h"
#include "appsettings.h"

class QTcpSocket;

// GetDataFromServer 连接CTPTraderServer 获得历史数据

class GetDataFromServer : public QObject
{
    Q_OBJECT
public:

    static GetDataFromServer* GetInstance( )           // 获取对象单例的指针
    {
        if(GetDataFromServer::m_getdata == nullptr)       // 如果单例对象没有创建， 则将其创建
        {
            GetDataFromServer::m_getdata = new GetDataFromServer( );
        }

        return GetDataFromServer::m_getdata;
    }

    static void DestroyInstance( )                  // 销毁单例对象的空间
    {
        if(GetDataFromServer::m_getdata != nullptr)
        {
            delete GetDataFromServer::m_getdata;
            GetDataFromServer::m_getdata = nullptr;
        }
    }

    //连接服务器
    void connectServer(QString host, quint16 port);


signals:
    // 获得到K线数据后通过信号与槽发送给数据对象
    void sendKlineBarData(QString symbol, KlineType type, QVector<BarData>);

public slots:
    // 连接后的槽函数
    void isConnected();
    void connectedError(QAbstractSocket::SocketError);

    //与服务器断开连接
    void disconnectServer();
    //重新连接
    void reconnectServer();
    //
    void DetectSocket();

    //接收服务器发送的数据
    void receiveData();
    // 老版本接收，现不用
    void receiveData(int);

    //向服务器发送数据
    void sendData(qint16 size, void *data);

private:
    explicit GetDataFromServer(QObject *parent = 0);
    GetDataFromServer(const GetDataFromServer &single) = delete;
    ~GetDataFromServer();
    static GetDataFromServer *m_getdata;

public:
    QTcpSocket *m_tcpClient;

private:
    //缓存上一次或多次的未处理的数据
    //这个用来处理，重新粘包
    QByteArray m_buffer;

    QString m_host;
    quint16 m_port;

    bool m_isConnect = false;           // 连接标志
    bool m_isReConnect = false;         // 重连标志

    //定时指针
private:
    QTimer *m_pReConnectTimer = nullptr;			// 重连定时
    QTimer *m_pDetectTimer = nullptr;				// 检测定时


    struct TickData{
        int begin_volume{ 0 };
        int end_volume{0};
        double open{ 0 };
        double high{ 0 };
        double low{ 0 };
        double close{ 0 };
        char begin_time[16];
        char end_time[16];
        char tradingDay[16];
        char symbol[32];
    };
};

#endif // GETDATAFROMSERVER_H
