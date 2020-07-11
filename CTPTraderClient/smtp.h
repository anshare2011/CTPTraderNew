#ifndef SMTP_H
#define SMTP_H

#include <QObject>
#include <QtCore>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QAbstractSocket>
#include <QDateTime>
#include <QDate>
#include <QLocale>


// qmake: QT += network
// 发送邮件

class Smtp : public QObject
{
    Q_OBJECT
public:
    explicit Smtp(QString smtphost, QString smtpusername, QString smtppass, QObject *parent = 0);
    ~Smtp();
    bool Send( const QString &to,const QString &subject, const QString &body );
    bool ReadLiner();
    void ErrorCloseAll();

signals:
    void status(const QString &);
    void SendLine();

public slots:
    bool PutSendLine();

private:
    QString SendLineAndGrab(QString senddata);
    QString encodeBase64( QString xml );
    QString decodeBase64( QString xml );
    int dateswap(QString form, uint unixtime );
    QString TimeStampMail();

    int linesend;
    QStringList ErrorMSG;

    int Timeout;
    bool isconnect;
    QString smtphost;
    QString smtpusername;
    QString smtppass;
    QString message;
    QString output;
    QString RemoteServerName;
    QString mailstatus;
    QTextStream *t;
    QTcpSocket *smtpsocket;
    QString from;
    QString rcpt;
    QString response;

};

#endif // SMTP_H
