#include "./hash.h"

//初始化
void Hash::init() {
	this -> len = 0;
	for (int i = 0; i < MAXN; i++) {
		this -> tail[i] = NULL;
	}
}

//返回哈希值
unsigned int Hash::hash (Work *work) {
	sockaddr_in *addr = (sockaddr_in *)work -> arg;
	char *p = inet_ntoa(addr -> sin_addr);
	unsigned int val = 0;

	for(int i = 0; i < strlen(p); i++) {
		val = val * 131 + p[i];
	}

	return val % MAXN;
}

//增加
void Hash::add (Work *work) {
	unsigned int val = hash(work);

	work -> next = this -> tail[val];
	this -> tail[val] = work;
	this -> len ++;

	return;
}

//删除
int Hash::del (Work *work) {
	unsigned int val = hash(work);
	sockaddr_in *addr = (sockaddr_in *)work -> arg;

	if (this -> tail[val] == NULL) {
		printf("del faild\n");
		return -1;
	}

	Work *p = this -> tail[val];
	sockaddr_in *addr1 = (sockaddr_in *)p -> arg;
	if (strcmp(inet_ntoa(addr1 -> sin_addr), inet_ntoa(addr -> sin_addr)) == 0 && htons(addr1 -> sin_port) == htons(addr -> sin_port)) {
		this -> tail[val] = p -> next;
		free(p -> arg);
		free(p);
		this -> len --;
		return 0;
	}

	Work *old = p;
	p = p -> next;
	while (p != NULL) {
		addr1 = (sockaddr_in *)p -> arg;
		if (strcmp(inet_ntoa(addr1 -> sin_addr), inet_ntoa(addr -> sin_addr)) == 0 && htons(addr1 -> sin_port) == htons(addr -> sin_port)) {
			old -> next = p -> next;
			free(p -> arg);
			free(p);
			this -> len --;
			return 0;
		}
		old = p;
		p = p -> next;
	}

	printf("del faild\n");
	return -1;
}

//全部删除
void Hash::alldel() {
	for (int i = 0; i < MAXN; i++) {
		Work *work = this -> tail[i];
		while(work != NULL) {
			Work *old = work;
			work = work -> next;
			free(old -> arg);
			free(old);
		}
	}

	return;
}

//查看
void Hash::getout () {
	printf("当前共有%d人在线，IP和端口号分别是：\n", this -> len);
	for (int i = 0; i < MAXN; i++) {
		Work *p = this -> tail[i];
		while(p != NULL) {
			sockaddr_in *addr = (sockaddr_in *)p -> arg;
			printf("%s %d\n", inet_ntoa(addr -> sin_addr), htons(addr -> sin_port));
			p = p -> next;
		}
	}
	
	return;
}






