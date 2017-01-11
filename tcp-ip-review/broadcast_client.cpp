/*
*broadcast_client.c - 多播的客户端
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define MCAST_PORT 8888
#define MCAST_INTERVAL 5                        /*发送间隔时间*/
#define BUFF_SIZE 256                           /*接收缓冲区大小*/

int main(int argc, char*argv[])
{
    int s;                                      /*套接字文件描述符*/
    struct sockaddr_in local_addr, from_addr;              /*本地地址*/
    int err = -1;

    s = socket(AF_INET, SOCK_DGRAM, 0);     /*建立套接字*/
    if (s == -1)
    {
        perror("socket()");
        return -1;
    }

    /*初始化地址*/
    memset(&local_addr, 0, sizeof(local_addr));

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);

    /*绑定socket*/
    err = bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        perror("bind()");
        return -2;
    }


    int times = 0;
    int addr_len = 0, from_len = 0;
    char buff[BUFF_SIZE];
    char str[32];
    int n = 0;

    /*循环接收多播组的消息，5次后退出*/
    for(times = 0;times<5;times++)
    {
        addr_len = sizeof(local_addr);
        from_len = sizeof(from_addr);
        memset(&from_addr, 0, sizeof(from_addr));
        memset(buff, 0, BUFF_SIZE);  /*清空接收缓冲区*/
        time_t  rtime;

        /*接收数据*/
        n = recvfrom(s, (void *)&rtime, sizeof(time_t), 0, (struct sockaddr *)&from_addr, (socklen_t *)&from_len);
        if( n== -1)
        {
            perror("recvfrom()");
        }
        rtime = ntohl(rtime);
        printf("received from %s at PORT %d => %ld\n",
               inet_ntop(AF_INET, &from_addr.sin_addr, str, sizeof(str)),
               ntohs(from_addr.sin_port), rtime);

//        sleep(MCAST_INTERVAL);
    }

    close(s);
    return 0;
}