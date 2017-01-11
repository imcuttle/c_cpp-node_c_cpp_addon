/*
*broadcast_server.c - 多播服务程序
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>

#define MCAST_PORT 8888
#define MCAST_ADDR "224.0.0.100"/    /*一个局部连接多播地址，路由器不进行转发*/
#define MCAST_DATA "BROADCAST TEST DATA"            /*多播发送的数据*/
#define MCAST_INTERVAL 5                            /*发送间隔时间*/

int main(int argc, char**argv)
{
    int s;
    struct sockaddr_in mcast_addr, local_addr;

    s = socket(AF_INET, SOCK_DGRAM, 0);         /*建立套接字*/
    if (s == -1)
    {
        perror("socket()");
        return -1;
    }



/*
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(0);

    bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr)) ;
*/


    memset(&mcast_addr, 0, sizeof(mcast_addr));/*初始化IP多播地址为0*/

    mcast_addr.sin_family = AF_INET;                /*设置协议族类行为AF*/
    inet_pton(AF_INET, "224.0.0.100", &mcast_addr.sin_addr);
    mcast_addr.sin_port = htons(MCAST_PORT);        /*设置多播端口*/
    /*向多播地址发送数据*/
    while(1) {
        time_t curr = time(NULL);
        int n = sendto(s,                           /*套接字描述符*/
                       (void *)&curr,     /*数据*/
                       sizeof(time_t),    /*长度*/
                       0,
                       (struct sockaddr*)&mcast_addr,
                       sizeof(mcast_addr)) ;
        if( n < 0)
        {
            perror("sendto()");
            return -2;

        }

        sleep(MCAST_INTERVAL);                          /*等待一段时间*/
    }

    return 0;
}