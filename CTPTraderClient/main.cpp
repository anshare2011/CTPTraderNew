#include "mainwindow.h"
#include "mainengine.h"
#include <QApplication>


// 全局变量
MainEngine* m_me = nullptr;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(m_me)
    {
        m_me->outputMessage(type, context, msg);
    }
}

int main(int argc, char *argv[])
{
    m_me = new MainEngine();
    qInstallMessageHandler(outputMessage);     //注册日志MessageHandler

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
