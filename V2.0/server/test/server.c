/*************************************************************************
	> File Name: server.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月17日 星期日 18时18分28秒
 ************************************************************************/

#include "../common/common.h"

#define MAXLINE 80
#define SERV_PORT 8888 
// 获得一个server的socket
int make_server_socket(int port,int q_size){

    // 获得一个socket
    int socket_id;
    if((socket_id = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("create socket wrong ");
        exit(1);
    }

    // 初始化地址端口
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置端口复用,不等待TIME_WAIT
    int yes = 1;
    if(setsockopt(socket_id,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
        close(socket_id);
        perror("setsockopt");
        return 1;
    }

    // 绑定端口
    if(bind(socket_id,(struct sockaddr*)&saddr,sizeof(saddr)) == -1){
        perror("wrong bind ");
        exit(1);
    }

    // 监听套接字
    if(listen(socket_id,q_size) == -1){
        perror("listen socket error: ");
        exit(1);
    }

    return socket_id;
}
 
int main(void){
    int sk = make_server_socket(SERV_PORT,10000);
    int i = 0;
    while (1) {
        int p = accept(sk,NULL,NULL);
        printf("%d \n",i++);
    }
}
