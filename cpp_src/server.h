//
// Created by Moyu on 16/10/9.
//

#ifndef START_SERVER_H
#define START_SERVER_H

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

void intHandler(int sig) {
    printf("Bye\n");
    wait(NULL);
    exit(1);
}

void server_run(const char* ip, int port) {

//    signal(SIGINT, intHandler);
    //创建套接字 (tcp->stream(字节流)   udp->datagram(数据报))
    // http://stackoverflow.com/questions/5815675/strange-thing-sock-dgram-and-sock-stream
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口
    if(-1 == bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        std::string str("Error bind on");
        char s[6];
        sprintf(s, "%d", port);
        str = str+ip+":"+s;
        perror(str.c_str());
        return;
    } else {
        printf("Success bind Server on %s:%d\n", ip, port);
    }

    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    //接收客户端请求
    sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    char str[] = "Hello World!";
    char strip[32];
    char buffer[40];

    int clnt_sock;


    while((clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_addr_size)) >= 0) {
        // 向客户端发送数据
        write(clnt_sock, str, sizeof(str));

        // 创建子进程，处理来自客户端的数据
        pid_t pid = fork();
        if(pid == 0) {
            int n;
            while(1) {
                n = recv(clnt_sock, buffer, sizeof(buffer), 0);
                if (n > 0){
                    printf("Message form client %s:%d: %s\n",
                           inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port), buffer);
                } else if(n < 0) {
                    perror("recv");
                    close(clnt_sock);
                    break;
                } else {
                    printf("FIN form client %s:%d\n",
                           inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port), buffer);
                    close(clnt_sock);
                    break;
                }
            }
        }
    }


    //关闭套接字
    close(serv_sock);
}

#endif //START_SERVER_H
