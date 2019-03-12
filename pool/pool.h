#ifndef _POOL_H_
#define _POOL_H_

#include "../hash/hash.h"

//线程池中线程执行的回掉函数
void* thread_pool_func(void *arg);

//线程池
struct thread_pool{
    //任务哈希表
    Hash *hash_user;
    //线程指针
    pthread_t *pd;

    //条件变量和锁。通过它控制每个任务有且只有一个线程执行
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    //线程池是否被销毁
    bool shutdown;

    //线程池中线程的总量
    int pthread_num;

    //初始化函数
    thread_pool(int pthread_num) {
        this -> pthread_num = pthread_num;
        this -> shutdown = 0;
        this -> hash_user = (Hash *)malloc(sizeof(Hash));
        this -> hash_user -> init();
        pthread_mutex_init(&this ->mutex, NULL);
        pthread_cond_init(&this -> cond, NULL);
        this -> pd = (pthread_t *)malloc(sizeof(pthread_t) * pthread_num);
        for (int i = 0; i < pthread_num; i++) {
            pthread_create(&this -> pd[i], NULL, thread_pool_func, this);
        }
    }

    //向线程池投递任务
    void add_work(Work *work);

    //销毁线程池
    int del();
};

#endif