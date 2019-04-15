/*************************************************************************
	> File Name: testEpoll.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年04月04日 星期四 19时16分07秒
 ************************************************************************/
#include"../common/common.h"
#define MAX_EVENTS 500

typedef struct evData{
    int fd;
    char buff[100];
}evData;

int main(){
    int server_listen;
    if((server_listen = socket_create(7731,10) < 0)){
        perror("socket_create ");
    }

    int epollfd = epoll_create(MAX_EVENTS);

    struct epoll_event ev,events[MAX_EVENTS];
    evData tmpData;
    tmpData.fd = server_listen;
    ev.data.ptr = &tmpData;
    ev.events = EPOLLIN;

    epoll_ctl(epollfd,EPOLL_CTL_ADD,server_listen,&ev);

    while(1){
        int nfds = epoll_wait(epollfd,events,MAX_EVENTS,-1);
        for(int i = 0;i < nfds;i ++){
            int fd = events[i].data.fd;
            if(fd == server_listen){
                int newFd = accept(server_listen,NULL,NULL);
                evData * tmp = malloc(sizeof(evData));
                tmp -> fd = newFd;
                ev.data.ptr = tmp;
                ev.events = EPOLLIN;
            }else if (events[i].events & EPOLLIN){
                evData * tmp = (void*)events[i].data.ptr;
            }
        }
    }
    return 0;
}
