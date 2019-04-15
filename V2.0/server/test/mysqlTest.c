/*************************************************************************
	> File Name: mysqlTest.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月23日 星期六 09时51分25秒
 ************************************************************************/
#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>

#define Fn_apply(type,fn,...){\
    void *stopper_for_apply = (int[]){0};\
    type **list_for_apply = (type*[]){__VA_ARGS__,stopper_for_apply};\
    for(int i = 0;list_for_apply[i] != stopper_for_apply;i ++){\
        fn(list_for_apply[i]);\
    }\
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

void queryDatabase(){
    char* query;
    asprintf(&query,"select * from warning");
} 

int inserData(mysqlClr*clr,char*tables,int tid,char* tname){
    char* query;
    asprintf(&query,"insert into %s (tname,tid) values('%s','%d')",tables,tname,tid);
    if(mysql_query(clr->mysql,query)){
        printf("query failde (%s)\n",query);
        return 0;
    }
    return 1;
}

#define pValue(...){printf(__VA_ARGS__);}
#define pName(tables,...){printf("insert into %s (%s) values",tables,#__VA_ARGS__);}
#define insert(arg,tables,...){pName(tables,__VA_ARGS__);pValue(arg,__VA_ARGS__)}

int main(){

    int a = 0;
    int b = 1;
    int c = 3;
    char d = 'c';

    insert("('%d','%d','%d','%c')\n","warning",a,b,c,d);

    mysqlClr clr;
    asprintf(&clr.server,"localhost");
    asprintf(&clr.user,"root");
    asprintf(&clr.passwd,"536842");
    asprintf(&clr.db,"piHealth");
    clr.port = 3306;

    if(!connectDatabase(&clr)){
        return -1;
    }

    freeMysqlClr(&clr);

    return 0;
}
