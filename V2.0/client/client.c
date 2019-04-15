/*************************************************************************
	> File Name: client.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 19时26分31秒
 ************************************************************************/

#include"common/common.h"

// 用来进程间同步用的
typedef struct sm_msg{
    int flag;
    int sm_time;
    pthread_mutex_t sm_mutex;
    pthread_cond_t sm_ready;
}sm_msg;

double memLogPar = 50;
int listenHeartPort = 0;
int listenChatPort = 0;
int loginToServerPort = 0;
int sendFileToServerPort = 0;
char* sIp = NULL;
char**map = NULL;
char* share_memory = NULL;
char pathName[] = "common/pihealth.conf";
sm_msg * msg = NULL;
int bigSize = 300;
char logFile[]="dailyLog/client.log";

#define WritePiLog(...){write_Pi_log(logFile,__VA_ARGS__);}
#define WritePiLogW(...){write_Pi_log(logFile,"\033[40;31m warning\033[0m"__VA_ARGS__);}
pthread_mutexattr_t m_attr;
pthread_condattr_t c_attr;

void initMapConfig(){
    map = malloc(sizeof(char*)*6);
    for(int i = 0;i < 12;i ++){
        map[i] = malloc(sizeof(char)*20);
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

void initConfig(){

    char* value = NULL;
    get_conf_value(pathName,"listenHeartPort",&value);
    listenHeartPort = atoi(value);
    get_conf_value(pathName,"listenChatPort",&value);
    listenChatPort = atoi(value);
    get_conf_value(pathName,"loginToServerPort",&value);
    loginToServerPort= atoi(value);
    get_conf_value(pathName,"sendFileToServerPort",&value);
    sendFileToServerPort= atoi(value);
    free(value);

    get_conf_value(pathName,"serverIp",&sIp);

    initMapConfig();
    return ;
}

//传输
void sendFile(int sockfd,FILE*f){
    char line[1024] = {0};
    char eot = EOF;
    int n;
    while(1){
        n = fread(line,sizeof(char),sizeof(line),f);
        if(send(sockfd,line,strlen(line),0)<0){
            return ;
        }
        if(n <= 0){
            send(sockfd,&eot,sizeof(char),0);
            break;
        }
    }
}

FILE* tarXZVF(char *fileName,char* tarName){
    char * commond;
    asprintf(&commond,"tar -xzvf ./log/backup/%s",tarName);
    fclose(popen(commond,"r"));
    free(commond);
    asprintf(&commond,"./log/backup/log/%s",fileName);
    FILE* f = fopen(commond,"r");
    free(commond);
    return f;
}

// TODO 测试
void sendAllFile(int sockfd,FILE* f,char*fileName){
    DIR *dp;
    struct dirent *ep;
    dp = opendir("./log/backup/");
    if(dp != NULL){
        while((ep = readdir(dp))){
            if(strncmp(ep->d_name,fileName,strlen(fileName)) == 0){
                FILE* fp = tarXZVF(fileName,ep->d_name);
                sendFile(sockfd,fp);
                fclose(fp);
            }
        }
        closedir(dp);
    }
    sendFile(sockfd,f);
}

void* listenServer(void*arg){
    int sockfd = make_server_socket(listenChatPort,10);
    WritePiLog("----Start  listenChatPort:%d----\n",listenChatPort);
    while(1){
        int cfd;
        if((cfd = accept(sockfd,NULL,NULL)) < 0){
            WritePiLogW("wrong accept");
            close(cfd);
            continue;
        }
        for(int i = 0;i < 6;i ++){
            fileMessage msg;
            char commond[100] = {0};
            // 收到服务器的信息
            recvMessage(cfd,&msg);
            
            char * logName = strdup(msg.content);
            // 打开相应的log 文件
            sprintf(commond,"log/%s.log",msg.content);
            FILE* op = fopen(commond,"r");
            sprintf(msg.name,"client[NO.%d]",msg.size);
            if(op == NULL){
                sprintf(msg.content,"someThings Wrong %s\n",strerror(errno));
                sendMessage(cfd,&msg);
                continue;
            }else{
                sprintf(msg.content,"OK");
                sendMessage(cfd,&msg);
            }
            // 接收传输命令
            recvMessage(cfd,&msg);

            if(strcmp(msg.content,"give me") == 0){
                sendAllFile(cfd,op,logName);
                WritePiLog("seccuess sendfile %s\n",commond);
            }

            fclose(op);
        }
        close(cfd);
    }
    close(sockfd);
}
// bash的执行间隔
int bashTimes[6] = {5,5,60,30,60,60};

// 该函数用来发送warning 信息
void sendWarningInfo(char * IP,int port,char*msg,int size){
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        WritePiLogW("wrong create socket");
        return ;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(IP);
    bzero(&(addr.sin_zero),8);

    sendto(sockfd,msg,size,0,(struct sockaddr*)&addr,sizeof(addr));

    close(sockfd);

    return ;
}

unsigned long get_file_size(const char *path){
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
       return filesize;
}

void tarFile(char* file,unsigned long int time){
    char commond[100];
    sprintf(commond,"tar -rf log/backup/%s.%lu.tar log/%s.log",file,time,file);
    pclose(popen(commond,"w"));
}

void work(void* arg){
    // 检查几次
    int times = 10;
    int bashNo;
    int bashTime;
    int pid;
    char file[100] = {0};
    char buff[4096] = {0};
    char commond[200] = {0};
    double dyaver = 50;
    // 开启六个子进程
    for(int i = 0;i < 6;i ++){
        // 标识进程
        bashNo = i;
        bashTime = bashTimes[i];
        sprintf(file,"log/%s.log",map[i]);
        sprintf(commond,"bash ./script/%s",map[i]);
        pid = fork();
        if(pid < 0){
            WritePiLogW("wrong work fork\n");
            continue;
        }
        if(pid == 0)break;
    }
    // 六个子进程
    if(pid == 0){
        int len = strlen(commond);
        int n = 0;
        while(1){
            for(int i = 0;i < times;i ++){
                if(bashNo == 1){
                    sprintf(commond,"%s %f",commond,dyaver);
                }
                char temp[400] = {0};
                FILE *op = popen(commond,"r");
                if(bashNo == 1){
                    commond[len] = '\0';
                    fgets(temp,sizeof(temp),op);
                    strcat(buff,temp);
                    fgets(temp,sizeof(temp),op);
                    dyaver = atof(temp);
                }else{
                    fread(temp,sizeof(char),sizeof(temp),op);
                    strcat(buff,temp);
                    if(bashNo == 5 && strstr(temp,"warning")){
                        WritePiLog("\033[47;31mfind warnig info \033[0m\n");
                        WritePiLog("%s\n",temp);
                        sendWarningInfo(sIp,8686,temp,strlen(temp));
                    }else if(bashNo == 4 && strlen(temp) > 0){
                        WritePiLog("\033[47;31mfind EVIL process \033[0m\n");
                        WritePiLog("%s\n",temp);
                        sendWarningInfo(sIp,8686,temp,strlen(temp));
                    }
                }
                pclose(op);
                sleep(bashTime);
                n ++;
                if(n * bashTime > 20){
                    FILE* fd = fopen(file,"a+");
                    fwrite(buff,sizeof(char),sizeof(buff),fd);
                    WritePiLog("finish %d work start to write in %s\n",n,file);
                    fclose(fd);
                    n = 0;
                    buff[0] = '\0';
                    if(get_file_size(file)/1024/1024 > bigSize){
                        // 压缩文件
                        tarFile(map[bashNo],time(NULL));
                        WritePiLog("清空 %s\n",map[bashNo]);
                        remove(file);
                    }
                }
                if(bashNo == 1){
                    msg->flag ++;
                    if(msg->flag > 9){
                        WritePiLogW("自检%d次，master 没有回应\n",msg->flag);
                        pthread_mutex_lock(&msg->sm_mutex);
                        pthread_cond_signal(&msg->sm_ready);
                        WritePiLog("准备呼唤心跳\n");
                        msg->sm_time = 10;
                        pthread_mutex_unlock(&msg->sm_mutex);
                        msg->flag = 0;
                    }
                }
            }
        }
    }
}

// 呼叫master
int call_master(char* ip,int port){
    int cfd = connect_to_ip(sIp,loginToServerPort);
    close(cfd);
    return cfd > 0 ? 1 : 0;
}

#define PRINT(arg) {WritePiLog("%s:%d\n",#arg,arg);}

int main(){
    /*
    int i = fork();
    if(i != 0)return 0;
    */

    initConfig();

    PRINT(listenHeartPort);
    PRINT(listenChatPort);
    PRINT(loginToServerPort);
    PRINT(sendFileToServerPort);
    WritePiLog("ServerIp:[%s]\n",sIp);
   
    // 连接一下Master 告诉他，我上线了。。
    int cfd = connect_to_ip(sIp,loginToServerPort);
    close(cfd);

    int shmid;
    if((shmid = shmget(IPC_PRIVATE,sizeof(struct sm_msg),0666|IPC_CREAT)) == -1){
        WritePiLogW("Erro in shmget:%s\n",strerror(errno)); 
        return -1;
    }

    if((share_memory = (char*) shmat(shmid,0,0)) == NULL){
        WritePiLogW("Erro in shmat:%s\n",strerror(errno)); 
        return -1;
    }

    msg = (struct sm_msg*)share_memory;
    msg -> flag = 0;

    if(pthread_mutexattr_init(&m_attr) < 0)WritePiLogW("Wrong p");
    
    if(pthread_condattr_init(&c_attr) < 0)WritePiLogW("Wrong p");

    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&msg->sm_mutex,&m_attr);
    pthread_cond_init(&msg->sm_ready,&c_attr);

    int pid = 0;

    if((pid = fork()) < 0){
        WritePiLogW("fork Error\n");
        return -1;
    }

    if(pid == 0){
        // 子进程
        WritePiLog("-->\t子进程:开始心跳监听[%d]\n",listenHeartPort);
        // 心跳监听
        int cfd = make_server_socket(listenHeartPort,10); 
        while(1){
            msg->flag = 1;
            int fd = accept(cfd,NULL,NULL);
            msg->flag = 0;
            WritePiLog("get hearBeat ok \n");
            if(fd < 0){
                WritePiLogW("wrong accept");
                continue;
            }
            close(fd);
        }
        close(cfd);

    }else if(pid > 0){
        // 父进程
        WritePiLog("->\t父进程:\n");
        int pidd = fork();
        if(pidd < 0){
            WritePiLogW("wron fork pidd \n");
            return -1;
        }else if(pidd == 0){
            WritePiLog("-->\t兄弟进程:等待主动连接Master\n")
            while(1){
                pthread_mutex_lock(&msg->sm_mutex);
                while(msg->sm_time < 2){
                    pthread_cond_wait(&msg->sm_ready,&msg->sm_mutex);
                }
                msg->sm_time = 10;
                int end = 1;
                while(end){
                    WritePiLog("开始呼唤master-[%d]次\n",msg->sm_time);
                    for(int i = 0;i < msg->sm_time;i ++){
                        WritePiLog(".... ");
                        sleep(1);
                        if(call_master(sIp,loginToServerPort)){
                            msg->flag = 0;
                            msg->sm_time = 0;
                            end = 0;
                            break;
                        }
                    }
                    msg->sm_time *= 2;
                }
                pthread_mutex_unlock(&msg->sm_mutex);
            }
        }else{
            WritePiLog("->\t父进程:\n");
            int piddd = fork();
            if(piddd < 0){
                WritePiLogW("wrong fork piddd\n");
                return -1;
            }else if(piddd == 0){
                WritePiLog("-->兄弟进程:开始自检工作\n"); 
                work(NULL);
            }else{
                WritePiLog("-->兄弟进程:等待Maste命令\n");
                listenServer(NULL);
            }
        }
    }

    return 0;
}
