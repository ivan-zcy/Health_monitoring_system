/*************************************************************************
	> File Name: testIptoInt.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月09日 星期六 11时05分29秒
 ************************************************************************/
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    char* from = "192.168.1.40";
    char* to = "192.168.3.11";
    struct in_addr addrFrom;
    struct in_addr addrTo;
    inet_aton(from,&addrFrom);
    inet_aton(to,&addrTo);
    unsigned int nfrom = ntohl(addrTo.s_addr);
    unsigned int nto =  ntohl(addrFrom.s_addr);
    for(unsigned i = nto;i < nfrom;i ++){
        struct in_addr p ;
        p.s_addr = htonl(i);
        printf("%s\n",inet_ntoa(p));
    }

}
