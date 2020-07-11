#ifndef CONFIGDEF_H
#define CONFIGDEF_H

#include <QSettings>

// 配置文件
#define CONFIG_FILE_NAME QString("./config/config.ini")

// server
#define SERVER_IP QString("Server/ip")
#define SERVER_PORT QString("Server/port")

// email
#define EMAIL_IS_SEND QString("email/isSend")
#define EMAIL_ACCOUNT QString("email/account")
#define EMAIL_PASSWORD QString("email/password")

// Database
#define DATABASE_PATH QString("Database/path")  //数据库存放路径，./为运行程序所在目录
#define DATABASE_FILE QString("Database/file")  //SQLITE数据库文件
#define DATABASE_NAME QString("Database/name")  //数据库名
#define DATABASE_USER QString("Database/user")  //用户名
#define DATABASE_PASSWD QString("Database/passwd")  //密码

#endif // CONFIGDEF_H
