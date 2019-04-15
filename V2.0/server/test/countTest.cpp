/*************************************************************************
	> File Name: countTest.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月16日 星期六 19时56分43秒
 ************************************************************************/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

void * creator(void * arg)
{
    int i = 0;
    while(i<300)
    {

        i++;
        cout << "creator add lock" << endl;
        pthread_mutex_lock(&mutex);

        count ++;

        cout << "in creator count is : " << count << endl;

        if(count > 0)
        {

            pthread_cond_signal(&cond);
        }


        cout << "creator release lock" << endl;

        pthread_mutex_unlock(&mutex);

    }

    return NULL;

}





void * consumer(void * arg)
{
    int i = 0;
    while(i < 100)
    {

        i++;
        cout << "consumer add lock" << endl;

        pthread_mutex_lock(&mutex);

        if(count <= 0)
        {
            cout << "begin wait" << endl;
            pthread_cond_wait(&cond,&mutex);
            cout << "end wait" << endl;
        }

        count --;

        cout << "in consumer count is " << count << endl;

        pthread_mutex_unlock(&mutex);

        cout << "consumer release lock" << endl;
    }

    return NULL;

}


int main()
{
     pthread_t createthread[2],consumethread[3];

     for(int i = 0; i < 3; i++)
     {
        pthread_create(&consumethread[i], NULL, consumer, NULL);
     }

     for(int i = 0; i < 2; i++)
     {
        pthread_create(&createthread[i], NULL, creator, NULL);
         }

     for(int i = 0; i < 2; i++)
      {
        pthread_join(createthread[i], NULL);
         }

     for(int i = 0; i < 3; i++)
     {
         pthread_join(consumethread[i], NULL);
     }


    return 0;

}
