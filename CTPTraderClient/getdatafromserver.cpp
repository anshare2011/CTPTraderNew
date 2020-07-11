#include "getdatafromserver.h"
#include <QtNetwork>
#include <QTcpSocket>

// 参考 https://www.cnblogs.com/findumars/p/5641848.html

GetDataFromServer* GetDataFromServer::m_getdata;

GetDataFromServer::GetDataFromServer(QObject *parent) : QObject(parent)
{
    m_isConnect = false;
    m_tcpClient = new QTcpSocket();

    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(m_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectedError(QAbstractSocket::SocketError)));
    connect(m_tcpClient,SIGNAL(connected()),this,SLOT(isConnected()));
    connect(m_tcpClient,SIGNAL(disconnected()),this,SLOT(disconnectServer()));
    connect(m_tcpClient,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

GetDataFromServer::~GetDataFromServer()
{
    m_tcpClient->disconnectFromHost();
    delete m_tcpClient;
}

void GetDataFromServer::isConnected()
{
    m_isConnect = true;
    qDebug()<< this->m_host<< this->m_port<< "is connected";
    // 连接成功，保存到配置文件
    appSettingsInstance->setSetting(SERVER_IP, this->m_host);
    appSettingsInstance->setSetting(SERVER_PORT, QString::number(this->m_port));

    //检测定时
    m_pDetectTimer = new QTimer;
    m_pDetectTimer->setSingleShot(false);
    connect(m_pDetectTimer, SIGNAL(timeout()), this, SLOT(DetectSocket()));
    m_pDetectTimer->start(4000);
}

void GetDataFromServer::connectedError(QAbstractSocket::SocketError error)
{
    qDebug()<< "GetDataFromServer connect to server failed";
}

void GetDataFromServer::connectServer(QString host, quint16 port)
{
    if(!m_isConnect)
    {
        m_host = host;
        m_port = port;
        qDebug()<< "GetDataFromServer::connectServer"<< host<< port;
        m_tcpClient->connectToHost(host, port);
    }
}

void GetDataFromServer::disconnectServer()
{
    //连接标志
    m_isConnect = false;

    //重连判断
    if(m_isReConnect == false)
    {
        //重连标记
        m_isReConnect = true;

        //重连定时
        m_pReConnectTimer = new QTimer;
        m_pReConnectTimer->setSingleShot(false);
        connect(m_pReConnectTimer, SIGNAL(timeout()), this, SLOT(reconnectServer()));
        m_pReConnectTimer->start(1000);
    }
}

void GetDataFromServer::reconnectServer()
{
    //删除定时
    if (m_pReConnectTimer != NULL)
    {
        delete m_pReConnectTimer;
        m_pReConnectTimer = NULL;
    }

    //重置标记
    m_isReConnect = false;

    m_tcpClient->close();
    m_tcpClient->connectToHost(this->m_host, this->m_port);
}

void GetDataFromServer::DetectSocket()
{

}

void GetDataFromServer::receiveData()
{
    qDebug() <<"GetDataFromServer::receiveData"<< m_tcpClient->bytesAvailable();
    int bytesAvailable = m_tcpClient->bytesAvailable();

    //缓冲区没有数据，直接无视
    if( m_tcpClient->bytesAvailable() <= 0 )
    {
        return;
    }

    //临时获得从缓存区取出来的数据，但是不确定每次取出来的是多少。
    QByteArray buffer;
    //如果是信号readyRead触发的，使用readAll时会一次把这一次可用的数据全总读取出来
    //所以使用while(m_tcpClient->bytesAvailable())意义不大，其实只执行一次。
    buffer = m_tcpClient->readAll();

    //上次缓存加上这次数据
    m_buffer.append(buffer);

    int totalLen = m_buffer.size();
    while(totalLen)
    {
        //不够包头的数据直接就不处理。
        if( totalLen < (int)sizeof(TCP_Head))
        {
            break;
        }
        // 解析包头
        TCP_Head *head = (TCP_Head *)m_buffer.data();
        // memcpy(m_buffer.data() , &head, sizeof(TCP_Head));

        int total_size = head->wPacketSize;                              // 完整包大小
        int avail_size = total_size - sizeof(struct TCP_Head);              // 除去包大小
        quint16 wMainCmdID = head->wMainCmdID;
        quint16 wSubCmdID = head->wSubCmdID;


        //如果不够长度等够了在来解析
        if( totalLen < total_size )
        {
            break;
        }

        switch (wMainCmdID)
        {
        case MDM_GR_KLINE:
            if(SUB_GR_KLINE_INFO == wSubCmdID)
            {
                GetKlineData *kline_info = (GetKlineData *)(m_buffer.data() + sizeof(TCP_Head));
                // memcpy(m_buffer.data() + sizeof(TCP_Head), &kline_info, sizeof(GetKlineData));
                avail_size -= sizeof(struct GetKlineData);

                QString symbol = QString(kline_info->symbol);
                KlineType type = KlineType::Min1;
                if(3 == kline_info->frequence)
                    type = KlineType::Min3;
                else if(5 == kline_info->frequence)
                    type = KlineType::Min5;
                else if(15 == kline_info->frequence)
                    type = KlineType::Min15;

                if(0 == avail_size % sizeof(TickData))
                {
                    // 读到的数据保存到 klineData
                    QVector<BarData> klineData;
                    for(int i=0; i<(avail_size)/ (int)sizeof(TickData); ++i)
                    {
                        TickData *data = (TickData *)(m_buffer.data() +sizeof(TCP_Head) + sizeof(GetKlineData) +sizeof(TickData) *i);
                        BarData oneKline;
                        oneKline.symbol = QString(data->symbol);
                        oneKline.begin_time = QString(data->begin_time);
                        oneKline.end_time = QString(data->end_time);
                        oneKline.time = oneKline.begin_time;
                        oneKline.currentMinute = oneKline.time.split(":").at(1).toInt();
                        oneKline.tradingDay = QString(data->tradingDay);

                        oneKline.volume_begin = data->begin_volume;
                        oneKline.volume_end = data->end_volume;
                        oneKline.volume = oneKline.volume_end - oneKline.volume_begin;
                        oneKline.open = data->open;
                        oneKline.high = data->high;
                        oneKline.low = data->low;
                        oneKline.close = data->close;
                        oneKline.is_new = false;
                        oneKline.is_one_tick = false;

                        klineData.push_back(oneKline);
                    }
                    // klineData 此处是时间倒序的
                    emit sendKlineBarData(symbol, type, klineData);
                }
                else
                {
                    qDebug() <<QString::fromLocal8Bit("GetDataFromServer::receiveData 数据大小异常");
                }
            }
            break;
        default:
            qDebug() << QString::fromLocal8Bit("GetDataFromServer::receiveData 接收到未知消息, 消息命令:") << wMainCmdID << wSubCmdID;
            break;
        }

        //缓存多余的数据
        buffer = m_buffer.right(totalLen - total_size);
        //更新长度
        totalLen = buffer.size();
        //更新多余数据
        m_buffer = buffer;

    }
}

void GetDataFromServer::receiveData(int s1)
{
    if(m_tcpClient->bytesAvailable() < sizeof(struct TCP_Head))
        return;
    TCP_Head head;
    m_tcpClient->read((char *)&head, sizeof(struct TCP_Head));

    int total_size = head.wPacketSize;
    int avail_size = total_size - sizeof(struct TCP_Head);
    quint16 wMainCmdID = head.wMainCmdID;
    quint16 wSubCmdID = head.wSubCmdID;
    qDebug() <<"GetDataFromServer::receiveData"<< head.wMainCmdID<< head.wSubCmdID<< m_tcpClient->bytesAvailable()<< avail_size;

    switch (wMainCmdID)
    {
    case MDM_GR_KLINE:
        if(SUB_GR_KLINE_INFO == wSubCmdID)
        {
            if(m_tcpClient->bytesAvailable() >= avail_size)
            {
                GetKlineData kline_info;
                m_tcpClient->read((char *)&kline_info, sizeof(struct GetKlineData));
                avail_size -= sizeof(struct GetKlineData);

                QString symbol = QString(kline_info.symbol);
                KlineType type = KlineType::Min1;
                if(3 == kline_info.frequence)
                    type = KlineType::Min3;
                else if(5 == kline_info.frequence)
                    type = KlineType::Min5;
                else if(15 == kline_info.frequence)
                    type = KlineType::Min15;

                if(0 == avail_size % sizeof(TickData))
                {
                    QByteArray ba1;
                    ba1.resize(avail_size);
                    m_tcpClient->read((char*)ba1.data() , avail_size);

                    QVector<BarData> klineData;
                    for(int i=0; i<(avail_size)/ (int)sizeof(TickData); ++i)
                    {
                        TickData *data = (TickData *)(ba1.data() +sizeof(TickData) *i);
                        BarData oneKline;
                        oneKline.symbol = QString(data->symbol);
                        oneKline.begin_time = QString(data->begin_time);
                        oneKline.end_time = QString(data->end_time);
                        oneKline.time = oneKline.begin_time;
                        oneKline.currentMinute = oneKline.time.split(":").at(1).toInt();
                        oneKline.tradingDay = QString(data->tradingDay);

                        oneKline.volume_begin = data->begin_volume;
                        oneKline.volume_end = data->end_volume;
                        oneKline.volume = oneKline.volume_end - oneKline.volume_begin;
                        oneKline.open = data->open;
                        oneKline.high = data->high;
                        oneKline.low = data->low;
                        oneKline.close = data->close;
                        oneKline.is_new = false;
                        oneKline.is_one_tick = false;

                        klineData.push_back(oneKline);
                    }
                    // klineData 此处是时间倒序的
                    emit sendKlineBarData(symbol, type, klineData);

                }
                else
                {
                    qDebug() <<"GetDataFromServer::receiveData data size error"<< avail_size<< sizeof(TickData);
                }
            }
            else
            {
                qDebug() << QString::fromLocal8Bit("可读的数据da大小不足");
            }
        }
        break;
    default:
        qDebug() << QString::fromLocal8Bit("接收到未知消息, 消息命令:") << wMainCmdID << wSubCmdID;
        break;
    }


    if(m_tcpClient->bytesAvailable())
        this->receiveData();

}


void GetDataFromServer::sendData(qint16 size, void *data)
{
    if(!m_isConnect) return;

    // 发送结构体
    if((m_tcpClient->write((char*)data, size)) != size)
    {
        qDebug()<< "send data to server failed!";
    }
    else{
        qDebug()<< "send data to server sucessed!";
    }
}
