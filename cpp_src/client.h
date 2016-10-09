//
// Created by Moyu on 16/10/9.
//

#ifndef START_CLIENT_H
#define START_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>

void client_run(const char* ip, int port) {
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口
    if(-1 == connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        std::string str("Error connect on");
        char s[6];
        sprintf(s, "%d", port);
        str = str+ip+":"+s;
        perror(str.c_str());
        return;
    } else {
        printf("Success connected on %s:%d\n", ip, port);
    }

    //读取服务器传回的数据
    char buffer[40];
    int n = read(sock, buffer, sizeof(buffer));
    if(n <= 0) {
        perror("Error read");
        close(sock);
        return;
    }

    printf("Message form server: %s\n", buffer);

    char ipt[40];
    while (true) {
        scanf("%s", ipt);
        if(write(sock, ipt, sizeof(ipt)) == -1) {
            break;
        }
    }
}


#endif //START_CLIENT_H
