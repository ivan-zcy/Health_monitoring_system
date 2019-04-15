# 设置c 语言的编译器
CC=gcc
CFLAGS=-g -Wall -fstack-protector-all

#common 需要的依赖
CommonOBJS=common.o

.PHONY:clean

mysqlHelp.o:common/mysqlHelp.c
	$(CC) $(CFLAGS) -c common/mysqlHelp.c -lmysqlclient

common.o: common/common.c
	$(CC) $(CFLAGS) -c common/common.c -lpthread 

server.o:server.c
	$(CC) $(CFLAGS) -c server.c -l pthread

# 创建server
server: common.o server.o mysqlHelp.o
	$(CC) $(CFLAGS) -o server mysqlHelp.o common.o server.o -lpthread -lmysqlclient


clean:
	rm *.o
