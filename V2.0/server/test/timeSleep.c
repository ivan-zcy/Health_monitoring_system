/*************************************************************************
	> File Name: timeSleep.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月19日 星期二 20时08分00秒
 ************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
 
void signalHandler(int signo)
{
    switch (signo){
        case SIGALRM:
            printf("Caught the SIGALRM signal!\n");
            break;
   }
}
 
int main(int argc, char *argv[])
{
    signal(SIGALRM, signalHandler);
 
    struct itimerval new_value, old_value;
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 1;
    new_value.it_interval.tv_sec = 3;
    new_value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &new_value, &old_value);

    while(1){
        
    }

    return 0;
}
