#ifndef _HASH_H_
#define _HASH_H_

#include "../tool/tool.h"

extern int const MAXN;

//用户结构体
struct User {
	sockaddr_in *addr;
	User *next;
};

//哈希结构体
struct Hash {	
	User* tail[100];
	int len;
	Hash() {
		this -> len = 0;
		for (int i = 0; i < MAXN; i++) {
			this -> tail[i] = NULL;
		}
	}
};

extern Hash hash_user;

//返回哈希值
unsigned int hash (User *user);

//增加
void add (User *user);

//删除
int del (User *user);

//查看
void getout ();

#endif