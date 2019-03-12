#ifndef _RWT_H_
#define _RWT_H_

#include "../pool/pool.h"

extern pthread_mutex_t rwt, mutex1, mutex2, mutex3, RWmutex;
extern int readCount, writeCount;

//初始化全局变量
void init();

//结束信号量
void goodby();

//读者
void Reader();

//写者
void Writer(Work *work, int flag);

//创建链接的任务函数
void* wel(void *t);

//定时查看任务
void* clock_get(void *t);

#endif