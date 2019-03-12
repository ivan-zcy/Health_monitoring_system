#include "pool.h"

//线程池中线程执行的回掉函数
void* thread_pool_func(void *arg) {
    thread_pool *t = (thread_pool *)arg;
    while(1) {
        //上锁
        pthread_mutex_lock(&t -> mutex);

        //循环等待接收任务
        while(!(t -> hash_user -> len) && !(t -> shutdown)) {
            pthread_cond_wait(&t -> cond, &t -> mutex);
        }

        //如果线程池被销毁
        if (t -> shutdown) {
            //解锁并退出线程
            pthread_mutex_unlock(&t -> mutex);
            pthread_exit(0);
        }

        //取出队首任务
        t -> hash_user -> len --;
        Work *p = NULL;
        for (int i = 0; i < MAXN; i++) {
            p = t -> hash_user -> tail[i];
            if (p != NULL) {
                t -> hash_user -> tail[i] = p -> next;
                break;
            }
        }

        //解锁
        pthread_mutex_unlock(&t -> mutex);

        //执行任务回掉函数
        (p -> func)(p -> arg);
        //释放该任务空间
        free(p);
    }
}

//向线程池投递任务
void thread_pool::add_work(Work *work) {
    pthread_mutex_lock(&this -> mutex);
    //将任务加入到等待队列
    unsigned int inx = this -> hash_user -> hash(work);
    Work *p = this -> hash_user -> tail[inx];
    if (p == NULL) {
        p = work;
    } else {
        while(p -> next != NULL) {
            p = p -> next;
        }
        p -> next = work;
    }
    this -> hash_user -> len ++;
    pthread_mutex_unlock(&this -> mutex);

    //唤醒一个等待线程
    pthread_cond_signal(&this -> cond);
}

//销毁线程池
int thread_pool::del() {
    if (this -> shutdown) {
        return -1;
    }
    this -> shutdown = 1;

    //唤醒所有线程
    pthread_cond_broadcast(&this -> cond);

    //等待所有线程结束,避免僵尸线程
    for (int i = 0; i < this -> pthread_num; i++) {
        pthread_join(this -> pd[i], NULL);
    }

    //释放掉所有线程标识符
    free(pd);

    //销毁任务队列的所有任务
    this -> hash_user -> alldel();
    free(this -> hash_user);

    //销毁条件变量和锁
    pthread_cond_destroy(&this -> cond);
    pthread_mutex_destroy(&this -> mutex);
    return 0;
}