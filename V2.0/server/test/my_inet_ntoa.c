/*************************************************************************
	> File Name: my_inet_ntoa.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月16日 星期六 09时56分24秒
 ************************************************************************/
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>


char* my_inet_ntoa(struct in_addr in){

    // 注意 使用 static 
    static char ans[16] = {0};
    uint32_t temp = in.s_addr;
    for(int i = 0;i < 4;i ++){
        sprintf(ans,"%s.%u",ans,temp&255);
        temp >>= 8;
    }

    // 更简单的转换方式
    char * p = (char*)&in.s_addr;
    printf("another_ntoa:%d.%d.%d.%d\n",p[0]&255,p[1]&255,p[2]&255,p[3]&255);

    return ans;
}

int main(){
    struct in_addr addr;
    inet_aton("192.168.1.1",&addr);
    printf("inet_ntoa:%s\n",inet_ntoa(addr));
    printf("my_inet_ntoa:%s\n",my_inet_ntoa(addr));
    return 0;
}
