/*************************************************************************
	> File Name: testSizeof.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月19日 星期二 20时58分35秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main(){
    char* a1 = malloc(sizeof(char)*10);
    int * a = malloc(sizeof(int) * 10);
    int b[10];
    printf("%lu\n",sizeof(*a1));
    printf("%lu\n",sizeof(*a));
    printf("%lu\n",sizeof(b));
}
