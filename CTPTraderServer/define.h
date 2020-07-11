#ifndef DEFINE_H
#define DEFINE_H

#include <QtSql>

//初始长度
#define init_size(Array) (sizeof(Array))

//拷贝长度
#define copy_size(Array) ((sizeof(Array) / sizeof(Array[0]))-1)

#endif // DEFINE_H
