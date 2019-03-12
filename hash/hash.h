#ifndef _HASH_H_
#define _HASH_H_

#include "../tool/tool.h"

extern int const MAXN;

//任务接口节点
struct Work {
    void* (*func) (void *arg);  //任务所执行的函数
    void *arg;         //任务所执行命令的参数，这里是sockaddr_in *
    Work *next;        //下一个任务
};

//哈希结构体
struct Hash {	
	Work* tail[100];
	int len;

	//初始化
	void init();

	//返回哈希值
	unsigned int hash (Work *work);

	//增加
	void add (Work *work);

	//删除
	int del (Work *work);

	//全部删除
	void alldel();

	//查看
	void getout ();
};

extern Hash Hash_User;

#endif