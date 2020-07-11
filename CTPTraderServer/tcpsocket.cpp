#include "tcpsocket.h"

TcpSocket::TcpSocket(int sock, QTcpSocket *parent): QTcpSocket(parent)
{
    this->setSocketDescriptor(sock);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(SocketErr(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(ReadAndParseData()));
    connect(this, SIGNAL(disconnected()), this, SLOT(dealDisconnect()));

    //检测定时
    m_detectTimer = new QTimer(this);
    m_detectTimer->setSingleShot(false);
    connect(m_detectTimer, SIGNAL(timeout()), this, SLOT(DetectSocket()));
    m_detectTimer->start(2000);


    m_wBufferSize = 0;

    quint32 value = QRandomGenerator::global()->generate();
    if(QSqlDatabase::contains(QString::number(value))){         // qsqlite    qt_sql_default_connection
        m_db = QSqlDatabase::database(QString::number(value));
    }else{
        m_db = QSqlDatabase::addDatabase("QSQLITE", QString::number(value));
        m_db.setDatabaseName("DataBase/SHFE.db");
    }

    // m_db.setDatabaseName("DataBase/SHFE.db");
    if (!m_db.open())
        qDebug() << "TcpSocket::TcpSocket open db failed";

}

TcpSocket::~TcpSocket()
{

}

void TcpSocket::DetectSocket()
{
    StartSendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, NULL, 0, false);
}

void TcpSocket::ReadAndParseData()
{
    if(this->bytesAvailable() < sizeof(struct TCP_Head))
    {
        return;
    }

    TCP_Head head;
    this->read((char *)&head, sizeof(struct TCP_Head));

    int total_size = head.wPacketSize;
    int avail_size = total_size - sizeof(struct TCP_Head);
    quint16 wMainCmdID = head.wMainCmdID;
    quint16 wSubCmdID = head.wSubCmdID;
    qDebug() <<"TcpSocket::head"<< head.wMainCmdID<< head.wSubCmdID<< this->bytesAvailable()<< avail_size;
    switch (wMainCmdID)
    {
    case MDM_GR_KLINE:
        if(SUB_GR_KLINE_INFO == wSubCmdID)
        {
            if(this->bytesAvailable() >= avail_size)
            {
                GetKlineData data;
                this->read((char *)&data, sizeof(struct GetKlineData));
                if(data.type == 0)
                {
                    QString table_name = QString(data.symbol);
                    if(data.frequence == KlineType::Min1)
                        table_name += "_min1";
                    else if(data.frequence == KlineType::Min3)
                        table_name += "_min3";
                    else if(data.frequence == KlineType::Min5)
                        table_name += "_min5";
                    else if(data.frequence == KlineType::Min15)
                        table_name += "_min15";

                    int limit = data.kline_size;

                    QString sql = QString("select symbol,open,high,low,close,begin_volume,end_volume,begin_time,end_time,tradingDay from %1 order by id desc limit %2;").arg(table_name).arg(limit);
                    qDebug()<< sql;
                    QSqlQuery query(sql, m_db);             // 查看db文件中的表名
                    int count_send = 0;
                    int count_query = 0;                     // query.size() 不可用
                    if (query.last())
                    {
                        count_query = query.at() + 1;
                    }
                    if(count_query < 1)
                        return;

                    query.first();          // 重新定位指针到结果集首位
                    query.previous();       // 如果执行query.next来获取结果集中数据，要将指针移动到首位的上一位。

                    TCP_Head head_back;
                    quint16 total_size_back = sizeof(TCP_Head) + sizeof(GetKlineData) + count_query * sizeof(TickData);
                    head_back.wPacketSize = total_size_back;
                    head_back.wMainCmdID = MDM_GR_KLINE;
                    head_back.wSubCmdID = SUB_GR_KLINE_INFO;

                    QByteArray ba_back;
                    ba_back.resize(total_size_back);
                    memcpy(ba_back.data() , &head_back, sizeof(TCP_Head));
                    memcpy(ba_back.data() + sizeof(TCP_Head), &data, sizeof(GetKlineData));

                    while (query.next()) {
                        QString symbol = query.value(0).toString();
                        double open = query.value(1).toDouble();
                        double high = query.value(2).toDouble();
                        double low = query.value(3).toDouble();
                        double close = query.value(4).toDouble();
                        int begin_volume = query.value(5).toDouble();
                        int end_volume = query.value(6).toDouble();
                        QString begin_time = query.value(7).toString();
                        QString end_time = query.value(8).toString();
                        QString tradingDay = query.value(9).toString();
                        // qDebug()<< symbol<< open<< high<< low<< close<< begin_volume<< end_volume<< begin_time<< end_time;

                        TickData data;
                        data.open = open;
                        data.high = high;
                        data.low = low;
                        data.close = close;
                        data.begin_volume = begin_volume;
                        data.end_volume = end_volume;
                        strncpy_s(data.symbol, symbol.toStdString().c_str(), sizeof(data.symbol));
                        strncpy_s(data.begin_time, begin_time.toStdString().c_str(), sizeof(data.begin_time));
                        strncpy_s(data.end_time, end_time.toStdString().c_str(), sizeof(data.end_time));
                        strncpy_s(data.tradingDay, tradingDay.toStdString().c_str(), sizeof(data.tradingDay));

                        memcpy(ba_back.data() + sizeof(TCP_Head) + sizeof(GetKlineData) + count_send * sizeof(TickData) , &data, sizeof(TickData));
                        count_send +=1;
                    }
                    qDebug() <<"TcpSocket::send"<< total_size_back<< count_query<< count_send;
                    this->write((char*)ba_back.data(), total_size_back);

                }
            }
        }
        break;
    default:
        qDebug() << QString::fromLocal8Bit("接收到未知消息, 消息命令:") << wMainCmdID << wSubCmdID;
        break;
    }

    if(this->bytesAvailable() > 0)
        this->ReadAndParseData();


    //调试信息
    //qDebug() << QString::fromLocal8Bit("处理数据请求, 线程标识") << QString::number(quintptr(QThread::currentThreadId()));

    //读取数据
    //    qint64 charCount = read(m_cbDataBuffer + m_wBufferSize, init_size(m_cbDataBuffer) - m_wBufferSize);

    //    //追加长度
    //    m_wBufferSize += charCount;

    //    //循环处理
    //    while ((m_wBufferSize >= sizeof(TCP_Head)))
    //    {
    //        //变量定义
    //        TCP_Info * pInfo = (TCP_Info *)m_cbDataBuffer;

    //        //长度校验
    //        if ((pInfo->wPacketSize < sizeof(TCP_Info)) || (pInfo->wPacketSize > SOCKET_TCP_BUFFER))
    //        {
    //            qDebug() << QString::fromLocal8Bit("数据包大小不对, 数据异常") << address;
    //            this->close();
    //            return;
    //        }

    //        //完整判断
    //        if (m_wBufferSize < pInfo->wPacketSize)
    //        {
    //            //继续接收
    //            break;
    //        }

    //        //变量定义
    //        quint16 wBufferDataSize = pInfo->wPacketSize;

    //        //拆解通信数据
    //        tagTcpData tcpData;
    //        if (m_tcpRecvSocketData.SetBufferData(m_cbDataBuffer, wBufferDataSize, tcpData) == false)
    //        {
    //            qDebug() << QString::fromLocal8Bit("设置数据包错误, 关闭连接") << address;
    //            this->close();
    //            return;
    //        }

    //        //处理消息
    ////		if (me->isTdConnect() == false)
    ////		{
    ////			qDebug() << QString::fromLocal8Bit("交易服务没有连接") << address;
    ////			return;
    ////		}

    //        //移动数据
    //        MoveMemoryCache(wBufferDataSize);

    //        //处理消息
    //        //me->onTcpSocketRead(address, tcpData.Command.wMainCmdID, tcpData.Command.wSubCmdID, tcpData.pDataBuffer, tcpData.wDataSize);
    //    }
}

void TcpSocket::dealConnect(QString address)
{
    this->address = address;
    // 获得客户端的地址
    qDebug() << QString::fromLocal8Bit("客户端地址") << address << QString::fromLocal8Bit("连接成功");
}

void TcpSocket::dealDisconnect()
{
    qDebug() << QString::fromLocal8Bit("客户端地址") << this->address << QString::fromLocal8Bit("断开连接");
    QMap<QString, int>::iterator it1;
    //	for (it1 = me->num_rec.begin(); it1 != me->num_rec.end(); ++it1)
    //	{
    //		if (it1.key().contains(this->address))
    //			it1.value() = 0;
    //	}
    //	for (it1 = me->num_real.begin(); it1 != me->num_real.end(); ++it1)
    //	{
    //		if (it1.key().contains(this->address))
    //			it1.value() = 0;
    //	}
    this->deleteLater();

    emit socketDisConnect(this->address);
}

void TcpSocket::SocketErr(QAbstractSocket::SocketError socketError)
{

}

// 发送数据
bool TcpSocket::StartSendData(quint16 wMainCmdID, quint16 wSubCmdID, const void *pData, quint16 wDataSize, bool bDelete)
{
    return true;
    //    if (m_tcpSendSocketData.InitSocketData(wMainCmdID, wSubCmdID, pData, wDataSize) == false)
    //    {
    //        qDebug() << QString::fromLocal8Bit("初始数据失败") << address;
    //        if (bDelete) delete pData;
    //        return false;
    //    }

    //    //发送数据
    //    quint64 ret = this->write((char*)&m_tcpSendSocketData.m_cbDataBuffer, m_tcpSendSocketData.m_wBufferSize);
    //    if (ret == m_tcpSendSocketData.m_wBufferSize)
    //    {
    //        if (bDelete) delete pData;
    //        return true;
    //    }
    //    else
    //    {
    //        qDebug() << QString::fromLocal8Bit("发送数据失败") << address;
    //        if (bDelete) delete pData;
    //        return false;
    //    }
}

void TcpSocket::MoveMemoryCache(quint16 wBufferDataSize)
{
    //数据缓存移动
    //    if (m_wBufferSize >= wBufferDataSize)
    //    {
    //        //正常移动
    //        m_wBufferSize -= wBufferDataSize;
    //        memmove(m_cbDataBuffer, m_cbDataBuffer + wBufferDataSize, m_wBufferSize);
    //    }
    //    else
    //    {
    //        //处理处理
    //        m_wBufferSize = 0;
    //        memset(m_cbDataBuffer, 0, sizeof(m_cbDataBuffer));
    //    }
}
