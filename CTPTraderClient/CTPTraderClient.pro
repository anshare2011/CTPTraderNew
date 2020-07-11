#-------------------------------------------------
#
# Project created by QtCreator 2020-03-04T01:23:46
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CTPTraderClient
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
    appsettings.cpp \
    backtestengine.cpp \
    ctaengine.cpp \
    getdatafromserver.cpp \
        mainwindow.cpp \
    riskmanagement.cpp \
    simulatedsettlement.cpp \
    simulateengine.cpp \
    smtp.cpp \
    strategy/strategybase.cpp \
    techindicator.cpp \
    ui/addipport.cpp \
    ui/backtest.cpp \
    ui/futuretabletop.cpp \
    ui/globalsetting.cpp \
    ui/login.cpp \
    ctpmdapi.cpp \
    ctptdapi.cpp \
    mainengine.cpp \
    dataengine.cpp \
    mywidgets/mypushbutton.cpp \
    mywidgets/mytabbar.cpp \
    mywidgets/mytabwidget.cpp \
    mywidgets/mywidget.cpp \
    tab_account_position.cpp \
    tab_order_trade.cpp \
    kline/autogrid.cpp \
    kline/klinegrid.cpp \
    kline/kvolumegrid.cpp \
    tradeboard.cpp \
    klinewidget.cpp \
    onesymbolklinedata.cpp \
    klinechart.cpp \
    mywidgets/mytoolbar.cpp \
    ui/searchinstrumentinformation.cpp \
    ui/sendorder.cpp \
    ui/setconditionorder.cpp \
    ui/setquicktrade.cpp \
    ui/searchmainwindow.cpp \
    klineheader.cpp \
    ui/setstopwinloss.cpp \
    ui/strategymanagement.cpp

HEADERS  += mainwindow.h \
    appsettings.h \
    backtestengine.h \
    configdef.h \
    ctaengine.h \
    ctpApi/ThostFtdcMdApi.h \
    ctpApi/ThostFtdcTraderApi.h \
    ctpApi/ThostFtdcUserApiDataType.h \
    ctpApi/ThostFtdcUserApiStruct.h \
    getdatafromserver.h \
    publicstruct.h \
    riskmanagement.h \
    simulatedsettlement.h \
    simulateengine.h \
    smtp.h \
    strategy/strategybase.h \
    talib/include/ta_libc.h \
    techindicator.h \
    ui/addipport.h \
    ui/backtest.h \
    ui/futuretabletop.h \
    ui/globalsetting.h \
    ui/login.h \
    ctpmdapi.h \
    ctptdapi.h \
    mainengine.h \
    dataengine.h \
    mywidgets/mypushbutton.h \
    mywidgets/mytabbar.h \
    mywidgets/mytabwidget.h \
    mywidgets/mywidget.h \
    tab_account_position.h \
    tab_order_trade.h \
    kline/autogrid.h \
    kline/klinegrid.h \
    kline/kvolumegrid.h \
    tradeboard.h \
    klinewidget.h \
    onesymbolklinedata.h \
    klinechart.h \
    mywidgets/mytoolbar.h \
    ui/searchinstrumentinformation.h \
    ui/sendorder.h \
    ui/setconditionorder.h \
    ui/setquicktrade.h \
    ui/searchmainwindow.h \
    klineheader.h \
    ui/setstopwinloss.h \
    ui/strategymanagement.h

FORMS    += mainwindow.ui \
    ui/addipport.ui \
    ui/backtest.ui \
    ui/futuretabletop.ui \
    ui/globalsetting.ui \
    ui/login.ui \
    ui/searchinstrumentinformation.ui \
    ui/sendorder.ui \
    ui/setconditionorder.ui \
    ui/setquicktrade.ui \
    ui/searchmainwindow.ui \
    ui/setstopwinloss.ui \
    ui/strategymanagement.ui

win32: LIBS += -L$$PWD/ctpApi/ -lthostmduserapi_se

INCLUDEPATH += $$PWD/ctpApi
DEPENDPATH += $$PWD/ctpApi

win32: LIBS += -L$$PWD/ctpApi/ -lthosttraderapi_se

INCLUDEPATH += $$PWD/ctpApi
DEPENDPATH += $$PWD/ctpApi

win32: LIBS += -L$$PWD/talib/lib/ -lta_libc_cdr

INCLUDEPATH += $$PWD/talib/include
DEPENDPATH += $$PWD/talib/include

