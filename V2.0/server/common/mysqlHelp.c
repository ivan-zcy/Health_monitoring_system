/*************************************************************************
	> File Name: mysqlHelp.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月24日 星期日 11时30分38秒
 ************************************************************************/

#include"mysqlHelp.h"

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


// fetchAll 封装完毕
int fetchAll(mysqlClr*clr,char* tables,void(*dofetch)(MYSQL_ROW,void*),void* arg){
    MYSQL* tmp = clr -> mysql;

    char* ctmp = tables;
    asprintf(&tables,"select * from %s",ctmp);
    free(ctmp);

    if(mysql_query(tmp,tables)){
        printf("wrong\n");
        return 0;
    }

    MYSQL_RES *res;     //行的一个查询结果集
    res = mysql_store_result(tmp);

    int i = 0;
    MYSQL_ROW column;   //数据行的列

    while(column = mysql_fetch_row(res)){
        dofetch(column,arg);
        i ++;
    }

    return i;
}

