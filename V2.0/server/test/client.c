/*************************************************************************
	> File Name: client.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月17日 星期日 18时16分15秒
 ************************************************************************/

#include "../common/common.h"
 
#define MAXLINE 80
#define SERV_PORT 8888
 
// 连接到IP
int connect_to_ip(char*IP,int port){
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("make socket:");
        return -1;
    }

    struct sockaddr_in their_add;
    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    if(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1){
        printf("connect server %s  failed:\n",IP);
        perror("");
        close(sockfd);
       return -1;
    }

    return sockfd;
}

int main(int argc){
    
    for(int i = 0;i < 88;i ++){
        connect_to_ip("192.168.2.86",SERV_PORT);
    }

    return 0;
}
