#include "rwt/rwt.h"

pthread_mutex_t rwt, mutex1, mutex2, mutex3, RWmutex;
int readCount, writeCount;
int const MAXN = 100;
Hash hash_user;

int main () {
	//获取配置信息
	char Server_string_port[10] = "";
	int Server_port;
	char Server_conf[20] = "./Server.conf";
	if (read_conf(Server_conf, "Server_port", Server_string_port) == -1) {
		exit(1);
	}
	Server_port = atoi(Server_string_port);

	//初始化信号量
	init();

	//初始化服务端
	int fd;
	if ((fd = Server_creat(Server_port)) == -1) {
		exit(1);
	}

	//打开一个定时查看哈希表信息的线程
	pthread_t clock_t;
	pthread_create(&clock_t, NULL, clock_get, NULL);

	while(1) {
		int newfd;
		pthread_t pd;
		User *user = (User *)malloc(sizeof(User));
		struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		socklen_t socklen = sizeof(struct sockaddr_in);
		
		//循环接收用户端连接
		if ((newfd = accept(fd, (struct sockaddr *)addr, &socklen)) == -1) {
			perror("accept faild");
			continue;
		}

		//创建用户信息并加入到哈希表中
		user -> addr = addr;
		user -> next = NULL;

		//分出一个线程来处理该用户
		pthread_create(&pd, NULL, wel, user);
	}

	//关掉套接字,信号量并退出
	pthread_join(clock_t, NULL);
	close(fd);
	goodby();
	return 0;
}