#include "./hash.h"

//返回哈希值
unsigned int hash (User *user) {
	char *p = inet_ntoa(user -> addr -> sin_addr);
	unsigned int val = 0;

	for(int i = 0; i < strlen(p); i++) {
		val = val * 131 + p[i];
	}

	return val % 97;
}

//增加
void add (User *user) {
	unsigned int val = hash(user);

	user -> next = hash_user.tail[val];
	hash_user.tail[val] = user;
	hash_user.len ++;

	return;
}

//删除
int del (User *user) {
	unsigned int val = hash(user);

	if (hash_user.tail[val] == NULL) {
		printf("del faild\n");
		return -1;
	}

	User *p = hash_user.tail[val];
	if (strcmp(inet_ntoa(p -> addr -> sin_addr), inet_ntoa(user -> addr -> sin_addr)) == 0 && htons(p -> addr -> sin_port) == htons(user -> addr -> sin_port)) {
		hash_user.tail[val] = p -> next;
		free(p -> addr);
		free(p);
		hash_user.len --;
		return 0;
	}

	User *old = p;
	p = p -> next;
	while (p != NULL) {
		if (strcmp(inet_ntoa(p -> addr -> sin_addr), inet_ntoa(user -> addr -> sin_addr)) == 0 && htons(p -> addr -> sin_port) == htons(user -> addr -> sin_port)) {
			old -> next = p -> next;
			free(p -> addr);
			free(p);
			hash_user.len --;
			return 0;
		}
		old = p;
		p = p -> next;
	}

	printf("del faild\n");
	return -1;
}

//查看
void getout () {
	printf("当前共有%d人在线，IP和端口号分别是：\n", hash_user.len);
	for (int i = 0; i < MAXN; i++) {
		User *p = hash_user.tail[i];
		while(p != NULL) {
			printf("%s %d\n", inet_ntoa(p -> addr -> sin_addr), htons(p -> addr -> sin_port));
			p = p -> next;
		}
	}
	return;
}






