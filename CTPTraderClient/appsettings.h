#ifndef APPSETTINGS_H
#define APPSETTINGS_H

/*
 * AppSettings 修改读取配置文件
 * 1.   读配置文件单例
 * 2.   参考 https://blog.csdn.net/w1820020635/java/article/details/88921359
 */

#include "configdef.h"

#define appSettingsInstance AppSettings::getInstance()

class AppSettings
{
public:
    static AppSettings* getInstance()
    {
        if(appSettings == nullptr)
        {
            appSettings = new AppSettings();
            settings = new QSettings(CONFIG_FILE_NAME,QSettings::IniFormat);
        }
        return appSettings;
    }

    static QVariant getSetting(QString key)
    {
        return settings->value(key);
    }

    static void setSetting(QString key, QVariant value)
    {
        return settings->setValue(key,value);
    }

private:
    AppSettings();                                      //禁止构造函数
    AppSettings(const AppSettings&){}                   //禁止拷贝构造函数
    AppSettings & operator=(const AppSettings &){}      //禁止赋值拷贝构造函数

private:
    static AppSettings *appSettings;
    static QSettings *settings;

};

#endif // APPSETTINGS_H
