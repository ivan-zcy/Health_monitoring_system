#include "./tool.h"

void out(int sig) {
	_exit(0);
}

//将格式化字符串format的内容写入PiHealthLog文件
int write_Pi_log(char *PiHealthLog, const char *format, ...) {
	FILE *file = NULL;

	//打开文件
	if ((file = fopen(PiHealthLog, "a+")) == NULL) {
		perror("fopen faild\n");
		return -1;
	}
	
	//初始化va_list
	va_list list;
	va_start(list, format);
	
	//获取时间
	time_t sec = time(NULL);
	struct tm *tblock = localtime(&sec);
	char *time = asctime(tblock);

	//将时间，格式化字符串format写入文件
	time[strlen(time) - 1] = '\0';
	fprintf(file, "%s ", time);
	vfprintf(file, format, list);

	//关掉va_lsit和文件
	va_end(list);
	fclose(file);
	return 0;
}

//初始化服务器并返回套接字
int Server_creat(int port) {
	//创建套接字
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1) {
		perror("socket faild");
		return -1;
	}

	//绑定端口
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		perror("bind faild");
		close(fd);
		return -1;
	}

	//进行监听
	if (listen(fd, 100) == -1) {
		perror("listen faild");
		close(fd);
		return -1;
	}

	return fd;
}

//初始化客户端并返回套接字
int User_creat(char *host, int port) {
	//创建套接字
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1) {
		perror("socket faild");
		return -1;
	}

	//连接对应服务器
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr))) {
		close(fd);
		return -1;
	}

	return fd;
}

//读取配置文件key对应的val值
int read_conf(char *filename, const char *key_name, char *val) {
	//打开配置文件
	char *buffer = NULL;
	char *p = NULL;
	FILE *file = NULL;
	ssize_t len = 0;
	size_t chang = 0;
	if ((file = fopen(filename, "r")) == NULL) {
		perror("fopen faild");
		return -1;
	}

	//循环读入每行信息到buffer
	while((len = getline(&buffer, &chang, file)) != -1) {
		//截取=前后两部分，若前半部分==key_name，那么val值为后半部分
		p = strtok(buffer, "=");
		if (strcmp(key_name, p) == 0) {
			p = strtok(NULL, "\n");
			strcpy(val, p);
		}
		while(p != NULL) {
			p = strtok(NULL, "\n");
		}
	}

	//关闭文件并判断是否成功读取
	fclose(file);
	if (strcmp(val, "") == 0) {
		printf("conf_file_value faild\n");
		return -1;
	}
	return 0;
}





