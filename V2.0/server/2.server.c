/*************************************************************************
	> File Name: server.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 21时04分35秒
 ************************************************************************/
#include"common/common.h"

#include"common/mysqlHelp.h"

#define WritePiLogInt(arg){write_Pi_log(logFile,#arg":[%d]\n",arg);}
#define WritePiLog(...){write_Pi_log(logFile,__VA_ARGS__);}

typedef struct argment{
    ServerControl* scf;
    clientInfo* cInfo;
    int index;
}argment;

int listenGetFilePort;
int listenLoginPort;
int chatToClientPort;
int heartToClientPort;
char*from;
char*to;
int INS;
mysqlClr clr;

const char*configFile="common/pihealthd.conf";
const char*logFile="log/Master.log";
char**map;

void initMapConfig(){
    map = (char**)malloc(sizeof(char*)*6);
    for(int i = 0;i < 6;i ++){
        map[i] =(char*) malloc(sizeof(char)*15);
    }
    strcpy(map[0],"CpuLog.sh");
    strcpy(map[1],"MemLog.sh");
    strcpy(map[2],"DiskLog.sh");
    strcpy(map[3],"Users.sh");
    strcpy(map[4],"ProcLog.sh");
    strcpy(map[5],"SysInfo.sh");
    for(int i = 0;i < 6;i ++){
        WritePiLog("Map %d : %s\n",i,map[i]);
    }
}

void print(ServerControl* scf){
    WritePiLog("ServerControl->INS:%d\n",scf->INS);

    for(int i = 0;i < scf->INS;i++){
        clientInfo*p = scf -> heads[i].head;
        int no = 0;
        while(p -> next){
            WritePiLog("%d-%d IP : [%s]\n",i,no++,inet_ntoa(p -> next ->saddr_client.sin_addr));
            p = p -> next;
        }
    }
}

int judgeINS(ServerControl*s){
    int temp = 0;
    for(int i = 0;i < s->INS;i ++){
        temp = (s->heads[i].len < s->heads[temp].len ? i:temp);
    }
    return temp;
}

void* serverAccept(void * arg){
    ServerControl*s = ((argment*)arg)->scf;
    clientInfo*cInfo = ((argment*)arg)->cInfo;
    char*ip = strdup(inet_ntoa(cInfo->saddr_client.sin_addr));
    int index = judgeINS(s); 
    WritePiLog("插入用户%s到队列%d\n",ip,index);
    addCinfo(s->heads+index,cInfo);
    free(ip);
    free(arg);
    return NULL;
}

void* heartBeatNoBlockSelect(void*arg){
    int len = sizeof(errno);
    ServerControl* s = (ServerControl*) arg;
    clientInfo* p; qheads* tempHead;
    // 超时时间设置
    // 0.5 秒
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000000 * 0.5;
    int n = 0;
    fd_set fds;
    while(1){
        // 文件描述符集合
        FD_ZERO(&fds);
        int maxFd = 0;
        WritePiLog("开始心跳-[%d]\n",n++);
        for(int i = 0;i < s->INS;i ++){
            tempHead = s -> heads+i;
            p = tempHead->head;
            while(p&&p->next){
                char*ip = strdup(inet_ntoa(p->next->saddr_client.sin_addr));
                int sfd = connect_to_ip_no_block(ip,heartToClientPort);
                WritePiLog("尝试连接%s:%d\n",ip,sfd);
                pthread_rwlock_rdlock(&tempHead->rwlock);
                p -> next -> sId = sfd;
                if(sfd == -1){
                    WritePiLog("connect to ip no block: %s\n",strerror(errno));
                    p = p -> next;
                    close(sfd);
                    continue;
                }
                p = p -> next;
                pthread_rwlock_unlock(&tempHead->rwlock);
                maxFd = maxFd > sfd ? maxFd:sfd;
                FD_SET(sfd,&fds);
                free(ip);
            } 
        }
        WritePiLog("开始select\n");
        int ret = select(maxFd+1,&fds,NULL,NULL,&tv);
        WritePiLog("ret = %d\n",ret);
        for(int i = 0;i < INS;i ++){
            tempHead = s -> heads+i;
            p = tempHead->head;
            while(p -> next){
                int sfd = p -> next -> sId;
                char*ip = strdup(inet_ntoa(p->next->saddr_client.sin_addr));
                WritePiLog("判断：IP %s\n",ip );
                if(FD_ISSET(sfd,&fds)){
                    getsockopt(sfd,SOL_SOCKET,SO_ERROR,&errno,(socklen_t*)&len);
                    if(errno != 0){
                        pthread_rwlock_wrlock(&tempHead->rwlock);
                        clientInfo*q = p -> next;
                        p->next = p -> next -> next;
                        tempHead -> len --;
                        free(q);
                        WritePiLog("删除节点%s\n",ip);
                        WritePiLog("Error : %s\n",strerror(errno));
                        print(s);
                        pthread_rwlock_unlock(&tempHead->rwlock);
                    }else{
                        WritePiLog("节点%s 还存在\n",ip);
                        p = p -> next;
                    }
                }else{
                    WritePiLog("超时%s\n",ip);
                    p = p->next;
                }
                close(sfd);
                free(ip);
            }
        }
    sleep(1);
    }
}

// 分配所有的IP
void initINSclient(ServerControl*s){
    struct in_addr addrFrom;
    struct in_addr addrTo;
    inet_aton(from,&addrFrom);
    inet_aton(to,&addrTo);
    unsigned int nfrom = ntohl(addrTo.s_addr);
    unsigned int nto =  ntohl(addrFrom.s_addr);
    for(unsigned i = nto;i <= nfrom;i ++){
        clientInfo* cInfo =(clientInfo*) malloc(sizeof(clientInfo));
        cInfo->saddr_client.sin_addr.s_addr = htonl(i);
        WritePiLog("生成配置IP:%s\n",inet_ntoa(cInfo->saddr_client.sin_addr));
        cInfo->next = NULL;
        addCinfo(s->heads+i%INS,cInfo);
        char filePath[20] = {0};
        sprintf(filePath,"file/%s",inet_ntoa(cInfo->saddr_client.sin_addr));
        mkdir(filePath,S_IRWXU);
    }
}

void initConfig(ServerControl*s){
    char*value;
    get_conf_value(configFile,"listenGetFilePort",&value);
    listenGetFilePort = atol(value);
    get_conf_value(configFile,"listenLoginPort",&value);
    listenLoginPort = atol(value);
    get_conf_value(configFile,"chatToClientPort",&value);
    chatToClientPort = atol(value);
    get_conf_value(configFile,"heartToClientPort",&value);
    heartToClientPort = atol(value);
    get_conf_value(configFile,"INS",&value);
    INS = atol(value);
    free(value);
    get_conf_value(configFile,"from",&from);
    get_conf_value(configFile,"to",&to);
}

// 从socket里接受数
// 存到FILE* 里
void saveToFile(int sd,FILE*f){
    char c;
    int n = 0;
    while((n = recv(sd,&c,sizeof(c),0))>0){
        if(c!=EOF){
            fputc(c,f);
        }else{
            break;
        }
    }
}

void chatToclient(char* ip){
    int fd;
    if((fd = connect_to_ip(ip,chatToClientPort))<0){
        WritePiLog("Wrong connect %s:%d\n",ip,chatToClientPort);
        return ;
    }
    for(int i = 0;i < 6;i ++){
        // 发送信息
        fileMessage message; 
        message.size = random()%1000;
        sprintf(message.name,"Master");
        sprintf(message.content,"%s",map[i]);
        sendMessage(fd,&message);
        WritePiLog("发送信息ok\n");
        // 接收信息
        recvMessage(fd,&message);
        // 如果收到了OK就回一个ok然后等待接受文件
        if(strcmp(message.content,"OK") == 0){
            char filePath[100] = {0};
            sprintf(filePath,"file/%s/%s.log",ip,map[i]);
            WritePiLog("Start write to file %s\n",filePath);

            FILE*f =  fopen(filePath,"a+");
            sprintf(message.name,"Master");
            sprintf(message.content,"give me");
            sendMessage(fd,&message);
            saveToFile(fd,f);
            fclose(f);

            WritePiLog("ok saveToFile %s\n",filePath);
        }else{
            // 记录一下
            write_Pi_log(logFile,"%s say:%s\n",message.name,message.content);
            WritePiLog("somethings Wrong\n");
        }
    }
    close(fd);
    return ;
}
// 工作函数 
void* work(void* arg_){
    argment* arg = (argment*) arg_;
    while(1){
        sleep(300);
        qheads*head = &arg -> scf -> heads[arg->index]; 
        clientInfo* p = arg ->scf -> heads[arg->index].head;
        while(p -> next){
            char* ip = strdup(inet_ntoa(p -> next -> saddr_client.sin_addr));
            WritePiLog("----------------------------------------------------\n");
            WritePiLog("** start to chat to %s ** \n",ip);
            chatToclient(ip);
            WritePiLog("** end chat to %s ** \n",ip);
            pthread_rwlock_rdlock(&head->rwlock);
            p = p -> next;
            pthread_rwlock_unlock(&head->rwlock);
            free(ip);
        }
    }
}

void* listenWarnigInfo(void*arg){

    int sid = socket(AF_INET,SOCK_DGRAM,0);
    if(sid < 0){
        WritePiLog("wrong create socket %s\n",strerror(errno));
        return NULL;
    }

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(*(int *)arg);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int yes = 1;
    if(setsockopt(sid,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0){
        close(sid);
        WritePiLog("setsockopt %s\n",strerror(errno));
        return NULL;
    }

    if(bind(sid,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        close(sid);
        WritePiLog("bind %s\n",strerror(errno));
        return NULL;
    }

    while(1){
        WritePiLog("start listenWarnigInfo_t \n");
        char recv[100] = {0};
        socklen_t len = sizeof(addr);
        recvfrom(sid,recv,sizeof(recv),0,(struct sockaddr*)&addr,&len);
        WritePiLog("\033[47;31mWaring\033[0m:%s\n",recv);
        char* wip = strdup(inet_ntoa(addr.sin_addr));
        char wtype[] = "1";
        char* wdetails = recv;
        inserDataNew(clr,"warning_events",wip,wtype,wdetails);
    }

    close(sid);

    return NULL;
}


void initMysqlClr(){
    asprintf(&clr.server,"localhost");
    asprintf(&clr.user,"root");
    asprintf(&clr.passwd,"536842");
    asprintf(&clr.db,"piHealth");
    clr.port = 3306;

    if(!connectDatabase(&clr)){
        WritePiLog("connect Database wrong %s\n",strerror(errno));
        exit(-1);
    }
}


int main(){
    ServerControl *s;

    initMapConfig();
    initConfig(NULL);
    initSCFL(&s,INS);
    initINSclient(s);
    initMysqlClr();

    WritePiLogInt(listenGetFilePort);
    WritePiLogInt(listenLoginPort);
    WritePiLogInt(chatToClientPort);
    WritePiLogInt(heartToClientPort);

    pthread_t heartBeatP;
    // 心跳的线程
    pthread_create(&heartBeatP,NULL,heartBeatNoBlockSelect,(void*)s);
    
    // 开始工作
    pthread_t* workIns = (pthread_t*)calloc(INS,sizeof(pthread_t));

    for(int i = 0;i < INS;i ++){
        argment*arg = (argment*)malloc(sizeof(argment));
        arg->scf = s;
        arg->index = i; 
        pthread_create(&workIns[i],NULL,work,(void*)arg);
    }

    int lsWport = 8686;
    // 监听UDP
    pthread_t listenWarnigInfo_t;
    pthread_create(&listenWarnigInfo_t,NULL,listenWarnigInfo,(void*)&lsWport);

    int sd = make_server_socket(listenLoginPort,10);
    socklen_t len = sizeof(struct sockaddr_in);

    while(1){
        int cfd;
        pthread_t pt;
        argment*arg = (argment*)malloc(sizeof(argment));
        clientInfo* cInfo = (clientInfo*)malloc(sizeof(clientInfo));
        if((cfd = accept(sd,(struct sockaddr*)&cInfo->saddr_client,&len) == -1)){
            WritePiLog("accept wrong %s\n",strerror(errno));
            free(cInfo);
            close(cfd);
            continue;
        }
        close(cfd);
        char*ip = strdup(inet_ntoa(cInfo->saddr_client.sin_addr));
        
        WritePiLog("IP:%s 用户登录\n",ip);
        
        if(findCinfo(s,ip,NULL)){
            WritePiLog("IP:%s 用户已经登录\n",ip);
            close(cfd);
            free(ip);
            continue;
        }
        arg->scf = s;
        arg->cInfo = cInfo;
        // 加入节点
        pthread_create(&pt,NULL,serverAccept,(void*)arg);
    }

    close(sd);
    freeMysqlClr(&clr);

    return 0;
}
