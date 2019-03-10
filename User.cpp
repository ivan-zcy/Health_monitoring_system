#include "tool/tool.h"

int main() {
	//两个进程面对ctrl+c执行out函数
	signal(SIGINT, out);

	char Server_host[20] = "";
	char Server_string_port[10] = "";
	char User_string_port[10] = "";
	int Server_port;
	int User_port;
	char User_conf[20] = "./User.conf";

	//获取配置信息
	if (read_conf(User_conf, "Server_port", Server_string_port) == -1) {
		exit(1);
	}
	if (read_conf(User_conf, "Server_host", Server_host) == -1) {
		exit(1);
	}
	if (read_conf(User_conf, "User_port", User_string_port) == -1) {
		exit(1);
	}
	Server_port = atoi(Server_string_port);
	User_port = atoi(User_string_port);

	//采用多进程
	int pid = fork();
	if (-1 == pid) {
		perror("fork faild\n");
	}

	if (!pid) {	//子进程用于绑定端口监听服务端
		int fd;
		int fatherpid = getppid();
		if (-1 == (fd = Server_creat(User_port))) {
			kill(fatherpid, SIGKILL);
			exit(1);
		}

		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int newfd;
		while(1) {
			if(-1 == (newfd = accept(fd, (sockaddr *)&addr, &len))) {
				continue;
			}
			printf("接收到链接\n");
		}

		close(fd);
		kill(fatherpid, SIGKILL);
		exit(0);
	} else {		//父进程用于客户端向服务端建立链接
		//初始化客户端并与服务端建立连接
		int fd;
		if (-1 == (fd = User_creat(Server_host, Server_port))) {
			exit(1);
		}

		//客户端对服务端做的操作
		char buffer[256];
		while(1) {
			scanf("%s", buffer);
			if (!strcmp(buffer, "quit")) {
				break;
			}
		}

		//关掉套接字并退出
		close(fd);
		kill(pid, SIGKILL);
		exit(0);
	}

	return 0;
}