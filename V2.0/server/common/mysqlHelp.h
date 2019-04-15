/*************************************************************************
	> File Name: mysqlHelp.h
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月24日 星期日 11时05分43秒
 ************************************************************************/

#ifndef _MYSQLHELP_H
#define _MYSQLHELP_H
#define _GNU_SOURCE
#pragma pack()
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>

#define Fn_apply(type,fn,...){\
    int tmp[] = {0};type *stopper_for_apply = (type*)tmp;\
    type* list_for_apply[] = {__VA_ARGS__,stopper_for_apply};\
    for(int i = 0;list_for_apply[i] != stopper_for_apply;i ++){\
        fn(list_for_apply[i]);}\
}

#define Free_all(...) Fn_apply(void,free,__VA_ARGS__);

typedef struct mysqlCLR{
    char * server;
    char * user;
    char * passwd;
    char * db;
    unsigned int port;
    MYSQL*mysql;
}mysqlClr;

#define coutFn(string){asprintf(&result,"%s'%s',",result,string );}

#define getInsertSQL(s,tables,...){\
    char* result;\
    asprintf(&result,"insert into %s (%s) values (",tables,#__VA_ARGS__);\
    Fn_apply(char,coutFn,__VA_ARGS__);\
    result[strlen(result) - 1] = ')';s = result;}

void freeMysqlClr(mysqlClr*clr);

int connectDatabase(mysqlClr* clr);

// 插入算是封装完了
#define inserDataNew(clr,...){\
    char* query;\
    getInsertSQL(query,__VA_ARGS__);\
    if(mysql_query(clr.mysql,query)){\
        printf("query failde %s\n",query);}\
    free(query);\
}

// fetchAll 封装完毕
int fetchAll(mysqlClr*clr,char* tables,void(*dofetch)(MYSQL_ROW,void*),void* arg);

#endif
