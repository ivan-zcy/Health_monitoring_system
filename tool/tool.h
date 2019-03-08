#ifndef _TOOL_H_
#define _TOOL_H_

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>    
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__);
#else 
#define DBG(...) 
#endif

//客户端ctrl+c执行函数
void out(int sig);

//将格式化字符串format的内容写入PiHealthLog文件
int write_Pi_log(char *PiHealthLog, const char *format, ...);

//初始化服务器并返回套接字
int Server_creat(int port);

//初始化客户端并返回套接字
int User_creat(char *host, int port);

//读取配置文件key对应的val值
int read_conf(char *file, const char *key, char *val);

#endif