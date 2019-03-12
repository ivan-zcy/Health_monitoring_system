#include "rwt/rwt.h"

pthread_mutex_t rwt, mutex1, mutex2, mutex3, RWmutex;
int readCount, writeCount;
int const MAXN = 100;
Hash Hash_User;

int main () {
	//获取配置信息
	char Server_string_port[10] = "";
	int Server_port;
	char Server_conf[20] = "./Server.conf";
	if (read_conf(Server_conf, "Server_port", Server_string_port) == -1) {
		exit(1);
	}
	Server_port = atoi(Server_string_port);

	//初始化线程池
	thread_pool pool(5);

	//初始化用户哈希表
	Hash_User.init();

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
		Work *work = (Work *)malloc(sizeof(work));
		struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
		socklen_t socklen = sizeof(struct sockaddr_in);
		
		//循环接收用户端连接
		if ((newfd = accept(fd, (struct sockaddr *)addr, &socklen)) == -1) {
			perror("accept faild");
			continue;
		}

		//创建任务信息节点
		work -> arg = (void *)addr;
		work -> func = wel;
		work -> next = NULL;

		//写入哈希表
		Writer(work, 0);

		//向线程池添加任务
		pool.add_work(work);
	}

	//关掉套接字,信号量，线程池并退出
	pthread_join(clock_t, NULL);
	close(fd);
	goodby();
	pool.del();
	return 0;
}