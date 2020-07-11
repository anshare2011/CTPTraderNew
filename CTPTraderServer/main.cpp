#include "mainwindow.h"
#include <QApplication>


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString level = "";
    switch(type)
    {
    case QtDebugMsg:
        level = QString("Debug");
        break;

    case QtWarningMsg:
        level = QString("Warning");
        break;

    case QtCriticalMsg:
        level = QString("Critical");
        break;

    case QtFatalMsg:
        level = QString("Fatal");

    default:
        level = QString("");
    }

    QString context_info = QString("F:(%1) L:(%2)").arg(QString(context.file)).arg(context.line); // F文件信息L行数
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("DateTime:%1, Level:%2, Message:%3, context:%4")
            .arg(current_date_time, level, msg, context_info);

    QString log_path = "log/";
    QDir temp;
    if (!temp.exists(log_path))
    {
        bool n = temp.mkpath(log_path);   //Creates the directory path dirPath.
    }

    // 文件名以日期进行区分
    QString filename = log_path + QDateTime::currentDateTime().toString("yyyy-MM-dd").append("-log.txt");
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();
    // 解锁
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册MessageHandler
    qInstallMessageHandler(outputMessage);

    MainWindow w;
    w.show();

    return a.exec();
}
