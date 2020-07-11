#-------------------------------------------------
#
# Project created by QtCreator 2020-03-04T01:22:43
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CTPTraderServer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    controlthread.cpp \
    showlocaldate.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    mainwindow.cpp \
    ctpmdapi.cpp \
    ctptdapi.cpp \
    login.cpp \
    mainengine.cpp \
    dataengine.cpp \
    dataserver.cpp \
    mydatabase.cpp \
    onesymbolklinedata.cpp \
    savedatetodatabase.cpp \
    timecontrol.cpp

HEADERS  += \
    controlthread.h \
    define.h \
    showlocaldate.h \
    tcpserver.h \
    tcpsocket.h \
    ctpApi/ThostFtdcMdApi.h \
    ctpApi/ThostFtdcTraderApi.h \
    ctpApi/ThostFtdcUserApiDataType.h \
    ctpApi/ThostFtdcUserApiStruct.h \
    mainwindow.h \
    ctpmdapi.h \
    ctptdapi.h \
    publicstruct.h \
    login.h \
    mainengine.h \
    dataengine.h \
    dataserver.h \
    mydatabase.h \
    onesymbolklinedata.h \
    savedatetodatabase.h \
    timecontrol.h

FORMS    += \
    mainwindow.ui \
    login.ui \
    showlocaldate.ui \
    timecontrol.ui

win32: LIBS += -L$$PWD/ctpApi/ -lthostmduserapi_se

INCLUDEPATH += $$PWD/ctpApi
DEPENDPATH += $$PWD/ctpApi

win32: LIBS += -L$$PWD/ctpApi/ -lthosttraderapi_se

INCLUDEPATH += $$PWD/ctpApi
DEPENDPATH += $$PWD/ctpApi
