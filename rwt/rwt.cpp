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
	Hash_User.getout();
	
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
void Writer(Work *work, int flag) {
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
		Hash_User.add(work);
	} else {
		Hash_User.del(work);
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

//心跳线程执行函数
void* clock_get(void *t) {
	//每隔5秒钟心跳检测一次，确定线程池中任务哈希表的所有链接是否存在
	char Server_conf[20] = "./Server.conf";
	char User_string_port[10] = "";
	int User_port;
	if (read_conf(Server_conf, "User_port", User_string_port) == -1) {
		pthread_exit(0);
	}
	User_port = atoi(User_string_port);
	//int User_port = 8732;

	while(1) {
		for (int i = 0; i < MAXN; i++) {
			Work *p = Hash_User.tail[i];
			while(p != NULL) {
				sockaddr_in *addr = (sockaddr_in *)p -> arg;
				int fd = User_creat(inet_ntoa(addr -> sin_addr), User_port);
				if (-1 == fd) {
					Writer(p, 1);
				} else {
					close(fd);
				}
				p = p -> next;
			}
		}
		Reader();
		sleep(5);
	}
	//退出线程
	pthread_exit(0);
}


void* wel(void *t) {
	printf("任务占位\n");
	return NULL;
}

