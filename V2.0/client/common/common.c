/*************************************************************************
	> File Name: common.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 18时45分53秒
 ************************************************************************/

#include"common.h"

/*
 * typedef struct clientInfo{
    struct sockaddr_in saddr_client;
    struct clientInfo* next;
}clientInfo;

typedef struct ServerControl{
    pthread_rwlock_t rwlock;
    int len;
    clientInfo* head;
}ServerControl;

 */

// 获得一个server的socket
int make_server_socket(int port,int q_size){

    // 获得一个socket
    int socket_id;
    if((socket_id = socket(AF_INET,SOCK_STREAM,0)) == -1){
        exit(1);
    }

    // 初始化地址端口
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置端口复用,不等待TIME_WAIT
    int yes = 1;
    if(setsockopt(socket_id,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
        close(socket_id);
        return 1;
    }

    // 绑定端口
    if(bind(socket_id,(struct sockaddr*)&saddr,sizeof(saddr)) == -1){
        exit(1);
    }

    // 监听套接字
    if(listen(socket_id,q_size) == -1){
        exit(1);
    }

    return socket_id;
}

// 连接到IP
int connect_to_ip(char*IP,int port){
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        return -1;
    }

    struct sockaddr_in their_add;
    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    if(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1){
        close(sockfd);
       return -1;
    }

    return sockfd;
}

// 初始化一下
void initSCFL(ServerControl** s,int INS){
    *s = malloc(sizeof(ServerControl));
    (*s) -> heads = calloc(INS,sizeof(qheads));
    (*s) -> INS = INS;
    for(int i = 0;i < INS;i ++){
        (*s) -> heads[i].head = malloc(sizeof(clientInfo)); 
        (*s) -> heads[i].head -> next = NULL; 
        (*s) -> heads[i].len  = 0; 
    }
    pthread_rwlock_init(&((*s)->rwlock),NULL);
}

// 根据IP查找一个用户节点
// 返回的查找到的值的上一个，这是为了删除方便
clientInfo* findCinfo(ServerControl*s,char* IP,int*index){
    for(int i = 0;i < s->INS;i++){
        clientInfo* p = s -> heads[i].head;
        pthread_rwlock_rdlock(&s->rwlock);
        while(p -> next != NULL ){
            char* pip = strdup(inet_ntoa(p -> next -> saddr_client.sin_addr));
            if(0 == strcmp(pip,IP)){
                pthread_rwlock_unlock(&s->rwlock);
                if(index != NULL)(*index = i);
                return p;
            }
            p = p -> next;
        }
        pthread_rwlock_unlock(&s->rwlock);
    }
    return NULL;
}

// 插入一个节点
void addCinfo(qheads*s,clientInfo* node){
    clientInfo* p = s -> head;

    pthread_rwlock_rdlock(&s->rwlock);
    while(p -> next!=NULL){
        //char*ip = strdup(inet_ntoa(p->next->saddr_client.sin_addr));
        p = p -> next;
    }
    pthread_rwlock_unlock(&s->rwlock);

    pthread_rwlock_wrlock(&s->rwlock);
    node->next = NULL;
    p -> next = node;
    s->len++;
    pthread_rwlock_unlock(&s->rwlock);
}

// 根据IP删除一个节点
void deleteCinfo(ServerControl*s,char* IP){
    int n = 0;
    clientInfo*p = findCinfo(s,IP,&n);
    if(p == NULL)return;
    pthread_rwlock_wrlock(&s->rwlock);
    clientInfo*q = p -> next;
    p -> next = p -> next -> next;
    free(q);
    s ->heads[n]. len --;
    pthread_rwlock_unlock(&s->rwlock);
}
// 获得从这个buff开始到结束的字符的字符串
char* get_a(char* buff,char endChar){
    int i = 0;
    while(buff[i]!=endChar)i++;
    char* result = calloc(i,sizeof(char));
    strncpy(result,buff,i);
    return result;
}

// 从配置文件里获得配置信息
int get_conf_value(char *pathname, char *key_name,char** value){

    FILE* fd = NULL;
    if((fd = fopen(pathname,"r")) == NULL){
        return -1;
    }

    char line[40] = {0};

    int i = 0;
    while(fgets(line,40,fd)){ 
        i = 0;
        while(key_name[i] && line[i] == key_name[i]){
            i ++ ;
        }
        if(line[i] == '='){
            line[strlen(line) - 1] = '\0';
            asprintf(value,"%s",line+i+1); 
            fclose(fd);
            return 0;
        }
    }
    fclose(fd);
    exit(-1);
}

// 向PiHealthLog 文件中写入内容，返回printed 的char的个数
// TODO 加文件锁
int write_Pi_log(char* PiHealthLog,const char*format,...){
    // 加入a / a+ 的区别
    FILE* fd = fopen(PiHealthLog,"a");
    time_t timep;struct tm *p;
    time(&timep);
    p=localtime(&timep);
    // 打印时间
    int n = fprintf(fd,"\033[47;30m[%04d-%02d-%02d %02d:%02d]\033[0m ",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
    va_list args;va_start(args,format);
    // fprintf 的变形可以传入va_list
    n += vfprintf(fd,format,args);
    // 刷新一下
    fflush(fd);
    va_end(args);fclose(fd);
    return n;
}
// 封装一下send 函数来发送一个Struct fileMessage
int sendMessage(int sockfd,fileMessage*msg){
// 将msg拷贝到char数组中然后发送
    int len=send(sockfd,(char*)msg,sizeof(fileMessage),0);
    if(len <= 0){
        return -1;
    }
    return 0;

}
// 封装一下recv 函数来接收一个Struct fileMessage
int recvMessage(int connetfd,fileMessage* msg){
    int len = recv(connetfd,(char*)msg,sizeof(fileMessage),0);
    if(len < 0){
        return -1;
    }
    return 0;
}

