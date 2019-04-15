# 设置c 语言的编译器
CC=gcc
CFLAGS=-g -Wall

#common 需要的依赖
CommonOBJS=common.o

.PHONY:clean

$(CommonOBJS): common/common.c
	$(CC) $(CFLAGS) -c common/common.c -lpthread

client.o:client.c
	$(CC) $(CFLAGS) -c client.c -lpthread

# 创建client
client: $(CommonOBJS) client.o
	$(CC) $(CFLAGS) -o client $(CommonOBJS) client.o -lpthread

clean:
	rm *.o
