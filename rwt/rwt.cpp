#include "./rwt.h"

//初始化全局变量
void init() {
	readCount = 0;
	writeCount = 0;
	pthread_mutex_init(&rwt, NULL);
	pthread_mutex_init(&RWmutex, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);
}

//结束信号量
void goodby() {
	pthread_mutex_destroy(&rwt);
	pthread_mutex_destroy(&RWmutex);
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);	
}


//读者
void Reader() {
	//入队列
	pthread_mutex_lock(&mutex3);
	pthread_mutex_lock(&RWmutex);
	pthread_mutex_lock(&mutex2);
	readCount ++;
	if (readCount == 1) {
		pthread_mutex_lock(&rwt);
	}
	pthread_mutex_unlock(&mutex2);
	pthread_mutex_unlock(&RWmutex);
	pthread_mutex_unlock(&mutex3);

	//读
	getout();
	
	//出队列
	pthread_mutex_lock(&mutex2);
	readCount --;
	if (readCount == 0) {
		pthread_mutex_unlock(&rwt);
	}
	pthread_mutex_unlock(&mutex2);

	return;
}

//写者
void Writer(User *user, int flag) {
	//入队列
	pthread_mutex_lock(&mutex1);
	writeCount ++;
	if (writeCount == 1) {
		pthread_mutex_lock(&RWmutex);
	}
	pthread_mutex_unlock(&mutex1);

	//写
	pthread_mutex_lock(&rwt);
	if (!flag) {
		add(user);
	} else {
		del(user);
	}
	pthread_mutex_unlock(&rwt);

	//出队列
	pthread_mutex_lock(&mutex1);
	writeCount --;
	if (writeCount == 0) {
		pthread_mutex_unlock(&RWmutex);
	}
	pthread_mutex_unlock(&mutex1);

	return;
}

//线程运行函数
void* wel(void *t) {
	//初始化用户
	User *user = (User *)t;
	int port = 8732;
	char *buffer = (char *)malloc(sizeof(char) * 256);
	
	//获取该用户hash值
	int val = hash(user);

	//添加用户
	Writer(user, 0);
	
	//每隔5秒钟心跳检测一次，确定链接是否存在
	while(1) {
		sleep(5);
		int fd = User_creat(inet_ntoa(user -> addr -> sin_addr), port);
		if (-1 == fd) {
			break;
		}
		close(fd);
	}

	//删除用户
	free(buffer);
	Writer(user, 1);
	
	//退出进程
	pthread_exit(0);
}

//定时查看任务
void* clock_get(void *t) {
	//循环查询
	while(1) {
		Reader();
		sleep(10);
	}

	//退出线程
	pthread_exit(0);
}





