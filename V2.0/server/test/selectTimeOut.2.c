/*************************************************************************
	> File Name: selectTimeOut.2.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月12日 星期二 20时58分48秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <netdb.h>
 
int my_tcp_connect(char *host_name, int port, int *sd, int timeout) {
    struct addrinfo hints;
    struct addrinfo *res;
    int result;
    char port_str[6];
    int flags = 0;
    fd_set rfds;
    fd_set wfds;
    struct timeval tv;
    int optval;
    socklen_t optlen;
 
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
 
    /* make sure our static port_str is long enough */
    if(port > 65535)
        return -1;
 
    snprintf(port_str, sizeof(port_str), "%d", port);
    result = getaddrinfo(host_name, port_str, &hints, &res);
    if(result != 0) {
        /*printf("GETADDRINFO: %s (%s) = %s\n",host_name,port_str,gai_strerror(result));*/
        return -1;
    }
 
    /* create a socket */
    *sd = socket(res->ai_family, SOCK_STREAM, res->ai_protocol);
    if(*sd < 0) {
        freeaddrinfo(res);
        return -1;
    }
 
    /* make socket non-blocking */
    flags = fcntl(*sd, F_GETFL, 0);
    fcntl(*sd, F_SETFL, flags | O_NONBLOCK);
 
    /* attempt to connect */
    result = connect(*sd, res->ai_addr, res->ai_addrlen);
 
    /* immediately successful connect */
    if(result == 0) {
        result = 1;
        /*printf("IMMEDIATE SUCCESS\n");*/
    }
 
    /* connection error */
    else if(result < 0 && errno != EINPROGRESS) {
        result = -1;
    }
 
    /* connection in progress - wait for it... */
    else {
 
        do {
            /* set connection timeout */
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
 
            FD_ZERO(&wfds);
            FD_SET(*sd, &wfds);
            rfds = wfds;
 
            /* wait for readiness */
            result = select((*sd) + 1, &rfds, &wfds, NULL, &tv);
 
            /*printf("SELECT RESULT: %d\n",result);*/
 
            /* timeout */
            if(result == 0) {
                /*printf("TIMEOUT\n");*/
                result = -1;
                break;
            }
 
            /* an error occurred */
            if(result < 0 && errno != EINTR) {
                result = -1;
                break;
            }
 
            /* got something - check it */
            else if(result > 0) {
 
                /* get socket options to check for errors */
                optlen = sizeof(int);
                if(getsockopt(*sd, SOL_SOCKET, SO_ERROR, (void *)(&optval), &optlen) < 0) {
                    result = -1;
                    break;
                }
 
                /* an error occurred in the connection */
                if(optval != 0) {
                    result = -1;
                    break;
                }
 
                /* the connection was good! */
                /*
                   printf("CONNECT SELECT: ERRNO=%s\n",strerror(errno));
                   printf("CONNECT SELECT: OPTVAL=%s\n",strerror(optval));
                   */
                result = 1;
                break;
            }
 
            /* some other error occurred */
            else {
                result = -1;
                break;
            }
 
        }
        while(1);
    }
 
    freeaddrinfo(res);
 
    return result;
}
 
int main(int argc, char *argv[])
{
    char *host_name = "www.baidu.com";
    int port = 80;
    int sd = 0;
    int timeout = 10;
    int ret = 0;
 
    ret = my_tcp_connect(host_name, port, &sd, timeout);
    if (ret >= 0) {
        fprintf(stdout, "OK\n");
    } else {
        fprintf(stdout, "ERROR\n");
    }
 
    while (1) {
        sleep(1);
    }
    return 0;
}
