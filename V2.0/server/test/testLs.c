/*************************************************************************
	> File Name: testLs.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月26日 星期二 19时08分15秒
 ************************************************************************/

#include <stddef.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <dirent.h> 

int main(void) { 

    DIR* dp; 
    struct dirent* ep; 
    dp = opendir("./"); 
    if (dp != NULL) { 
        while (ep = readdir(dp)) 
            printf("%s\n",ep->d_name); 
        (void) closedir(dp); 
    }else 
        puts("Couldn't open the directory."); 
    return 0; 
} 

