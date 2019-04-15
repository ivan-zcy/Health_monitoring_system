/*************************************************************************
	> File Name: mysqlTest.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月23日 星期六 09时51分25秒
 ************************************************************************/
#include<iostream>
#include<string>
#include<string.h>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<mysql/mysql.h>

using namespace std;

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

#define coutFn(string){sin <<"'" << string << "',";}

#define getInsertSQL(s,tables,...){\
    ostringstream sin;\
    sin << "insert into "<<tables<<" ("<<#__VA_ARGS__<<") values (";\
    Fn_apply(char,coutFn,__VA_ARGS__);\
    s = sin.str();s[s.length() - 1] = ')';}

// 插入算是封装完了
#define inserDataNew(clr,...){\
    string query;\
    getInsertSQL(query,__VA_ARGS__);\
    if(mysql_query(clr.mysql,query.c_str())){\
        cout << "query failde " << query << endl;}\
}

// fetchAll 封装完毕
int fetchAll(mysqlClr*clr,string tables,void(*dofetch)(MYSQL_ROW,void*),void* arg){
    MYSQL* tmp = clr -> mysql;
    string a = "select * from "+tables;

    if(mysql_query(tmp,a.c_str())){
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

typedef struct types{
    char* tid;
    char* tname;
}types;

void getTypes(MYSQL_ROW c,void* arg_){
    vector<types*>* arg = (vector<types*>*)arg_;
    types* tmp = new types();
    tmp->tid = strdup(c[0]);
    tmp->tname = strdup(c[1]);
    arg -> push_back(tmp);
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

    char tid[] = "1234";
    char tname[] = "qqq";

    inserDataNew(clr,"types",tid,tname);

    vector<types*> t;

    fetchAll(&clr,"types",getTypes,(void*)&t);

    for(auto item:t){
        printf("%s:%s\n",item->tid,item->tname);
    }

    freeMysqlClr(&clr);

    return 0;
}
