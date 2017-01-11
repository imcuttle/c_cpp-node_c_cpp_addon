/*
*broadcast_server.c - 多播服务程序
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>

#define BCAST_PORT 8888
#define BCAST_ADDR "255.255.255.255"    /*一个局部连接多播地址，路由器不进行转发*/
#define BCAST_DATA "BROADCAST TEST DATA"            /*多播发送的数据*/
#define BCAST_INTERVAL 3                            /*发送间隔时间*/
#define BUFLEN 256

void getcurtime(char* curtime)
{
    time_t tm;
    time(&tm);
    snprintf(curtime, BUFLEN, "%s", ctime(&tm));
}


int main(int argc, char**argv)
{
    int s, on = 1;
    struct sockaddr_in bcast_addr, local_addr;
    char buffer[BUFLEN];

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);         /*建立套接字*/


    // 允许广播
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));

    if (s == -1)
    {
        perror("socket()");
        return -1;
    }

    memset(&bcast_addr, 0, sizeof(bcast_addr));/*初始化IP多播地址为0*/
    char str[32];

    bcast_addr.sin_family = AF_INET;                /*设置协议族类行为AF*/
//    bcast_addr.sin_addr.s_addr = inet_addr(BCAST_ADDR);
    inet_pton(AF_INET, BCAST_ADDR, &bcast_addr.sin_addr);
    bcast_addr.sin_port = htons(BCAST_PORT);        /*设置多播端口*/

//    local_addr.sin_family = AF_INET;
////    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    inet_pton(AF_INET, "172.22.200.59", &local_addr.sin_addr);
//    local_addr.sin_port = htons(BCAST_PORT);
//    if(0<bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr))) {
//        perror("bind");
//    }
    /*向多播地址发送数据*/
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        time_t curtime = time(NULL);
        getcurtime(buffer);
        printf("Send %ld \n", curtime);

        curtime = htonl(curtime);

        int n = sendto(s,                    /*套接字描述符*/
                       (void *)&curtime,     /*数据*/
                       sizeof(curtime),    /*长度*/
                       0,        /*MSG_DONTROUTE配置出错，需要经过路由器转发*/
                       (struct sockaddr*)&bcast_addr,
                       sizeof(bcast_addr));

        if( n < 0)
        {
            perror("sendto()");
            return -2;
        }
        printf("Send To %s at PORT %d\n",
               inet_ntop(AF_INET, &bcast_addr.sin_addr, str, sizeof(str)),
               ntohs(bcast_addr.sin_port));

        sleep(BCAST_INTERVAL);                          /*等待一段时间*/
    }

    return 0;
}