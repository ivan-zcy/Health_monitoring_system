/*************************************************************************
	> File Name: mysqlTest.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月23日 星期六 09时51分25秒
 ************************************************************************/
#define _GNU_SOURCE
#pragma pack()
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>
#include<time.h>
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

void freeMysqlClr(mysqlClr*clr){
    Free_all(clr->server,clr->user,clr->passwd,clr->db); 
    mysql_close(clr->mysql);
}

int connectDatabase(mysqlClr* clr){
    clr->mysql = mysql_init(NULL);
    if(mysql_real_connect(clr->mysql,clr->server,clr->user,clr->passwd,clr->db,0,NULL,0)){
        return 1;
    }else{
        perror("wrong connect");
        return 0;
    }
}

#define coutFn(string){asprintf(&result,"%s'%s',",result,string );}

#define getInsertSQL(s,tables,...){\
    char* result;\
    asprintf(&result,"insert into %s (%s) values (",tables,#__VA_ARGS__);\
    Fn_apply(char,coutFn,__VA_ARGS__);\
    result[strlen(result) - 1] = ')';s = result;}

// 插入算是封装完了
#define inserDataNew(clr,...){\
    char* query;\
    getInsertSQL(query,__VA_ARGS__);\
    if(mysql_query(clr.mysql,query)){\
        printf("query failde %s\n",query);}\
    free(query);\
}


// 这个是没啥用
#define ForEach(iterator,...)\
        char* iterator_i[] = {__VA_ARGS__,NULL};\
        char** iterator = (char**) iterator_i;\
        for(;*iterator;iterator++)

int main(){

    mysqlClr clr;
    asprintf(&clr.server,"localhost");
    asprintf(&clr.user,"root");
    asprintf(&clr.passwd,"536842");
    asprintf(&clr.db,"piHealth");
    clr.port = 3306;

    if(!connectDatabase(&clr)){
        return -1;
    }

    char wip [20] = "192.16.2.1";
    char wtype[20] = "1";
    char  wdetails[20] = "test";

    inserDataNew(clr,"warning_events",wip,wtype,wdetails);

    freeMysqlClr(&clr);

    return 0;
}
