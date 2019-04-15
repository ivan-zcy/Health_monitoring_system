/*************************************************************************
	> File Name: common.h
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 18时44分51秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
#define _GNU_SOURCE
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdarg.h>
#define DEBUG

#if defined DEBUG
#define DBG(...) {printf(__VA_ARGS__);}
#else
#define DBG(...) {}
#endif



// 传输文件的结构体
typedef struct fileMessage{
    int size;
    char name[20];
    char content[1024];
}fileMessage;

// 每个用户的信息
typedef struct clientInfo{
    struct sockaddr_in saddr_client;
    int sId;
    int clength;
    char name[20];
    struct clientInfo* next;
}clientInfo;

// 用来控制每个链表
typedef struct queueHeads{
    pthread_rwlock_t rwlock;
    int len;
    clientInfo*head;
}qheads;

typedef struct ServerControl{
    pthread_rwlock_t rwlock;
    // 并发度
    int INS;
    // 头列表
    qheads* heads;
}ServerControl;

// 获得一个server的socket
int make_server_socket(int port,int q_size);
// 连接到IP
int connect_to_ip(char*ip,int port);
// 非阻塞连接到IP
int connect_to_ip_no_block(char*ip,int port);
// 初始化一下
void initSCFL(ServerControl** s,int INS);
// 根据IP查找一个用户节点
clientInfo* findCinfo(ServerControl*s,char* IP,int*a);
// 插入一个节点
void addCinfo(qheads*s,clientInfo* node);
// 根据IP删除一个节点
void deleteCinfo(ServerControl*s,char* IP);
// 从配置文件里获得配置信息
int get_conf_value(const char *pathname,const char *key_name,char**);

// 封装一下send 函数来发送一个Struct fileMessage
int sendMessage(int sockfd,fileMessage*msg);
// 封装一下recv 函数来接收一个Struct fileMessage
int recvMessage(int connetfd,fileMessage* msg);

// 向PiHealthLog 文件中写入内容 返回成功print的char的个数
// TODO 加文件锁
int write_Pi_log(const char *PiHealthLog,const char*format,...);

#endif
