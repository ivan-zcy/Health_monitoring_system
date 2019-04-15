/*************************************************************************
	> File Name: selectTimeOut.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月12日 星期二 20时52分18秒
 ************************************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
 
int main(int argc, char *argv[]) // 注意输入参数, 带上ip和port
{
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
    struct sockaddr_in addrSrv;
    addrSrv.sin_addr.s_addr = inet_addr("192.168.5.22");
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(atoi("8800"));
 
	fcntl(sockClient, F_SETFL, fcntl(sockClient, F_GETFL, 0)|O_NONBLOCK);  
	
    int iRet = connect(sockClient, ( const struct sockaddr *)&addrSrv, sizeof(struct sockaddr_in));
	printf("connect iRet is %d, errmsg:%s\n", iRet, strerror(errno)); // 返回-1不一定是异常
 
	if (iRet != 0)  
	{  
	   	if(errno != EINPROGRESS)
		{
			printf("connect error:%s\n", strerror(errno));  
		}
	   	else  
		{
			struct timeval tm = {5, 0};	
			fd_set wset, rset;  
			FD_ZERO(&wset);	
			FD_ZERO(&rset);	
			FD_SET(sockClient, &wset);  
			FD_SET(sockClient, &rset); 
			int time1 = time(NULL);
			int n = select(sockClient + 1, &rset, &wset, NULL, &tm);  
			int time2 = time(NULL);
			printf("time gap is %d\n", time2 - time1);
 
			if(n < 0)	
			{  
			   printf("select error, n is %d\n", n);  
			}  
			else if(n == 0)  
			{  
			   printf("connect time out\n");  
			}  
			else if (n == 1)  
			{
			   if(FD_ISSET(sockClient, &wset))	
			   {  
				   printf("connect ok!\n");  
				   fcntl(sockClient, F_SETFL, fcntl(sockClient, F_GETFL, 0) & ~O_NONBLOCK);  
			   }  
			   else  
			   {  
				   printf("unknow error:%s\n", strerror(errno));	
			   }  
			}
			else
			{
				printf("oh, not care now, n is %d\n", n);
			}
		}  
	}  
 
	printf("I am here!\n");
    getchar();
    close(sockClient);
    return 0;
}
