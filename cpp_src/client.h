//
// Created by Moyu on 16/10/9.
//

#ifndef START_CLIENT_H
#define START_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>


#include "command.h"

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

    char buffer[40];
    int maxfd = sock, rRes, wRes, r1Res;
    fd_set sockSet, inSet, rSet, wSet, r1Set;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    FD_ZERO(&sockSet);
    FD_ZERO(&inSet);
    FD_SET(sock, &sockSet);
    FD_SET(STDIN_FILENO, &inSet);


    char wBuffer[1024];
    while(1) {
        r1Set = inSet;
        rSet = sockSet;
        wSet = sockSet;

        rRes = select(maxfd+1, &rSet, NULL, NULL, &timeout);

        if(rRes > 0 && FD_ISSET(sock, &rSet)) {
//            memset(buffer, 0, sizeof(buffer));
            int n = read(sock, buffer, sizeof(buffer));
            if(n < 0) {
                perror("Error read");
                close(sock);
                return;
            } else if(n == 0) {
                printf("Server closed.");
                FD_CLR(sock, &sockSet);
                close(sock);
                return;
            }
            buffer[n] = '\0';
            printf("Message form server: %s\n", buffer);
        }

        r1Res = select(maxfd+1, &r1Set, NULL, NULL, &timeout);

        if(r1Res > 0 && FD_ISSET(STDIN_FILENO, &r1Set)) {
            int n = read(STDIN_FILENO, buffer, sizeof(buffer));
            buffer[n-1] = '\0';

            if(n < 0) {
                perror("Error read");
                FD_CLR(STDIN_FILENO, &inSet);
                close(sock);
                return;
            }
            strcat(wBuffer, buffer);
        }

        wRes = select(maxfd+1, NULL, &wSet, NULL, &timeout);
        if(wRes > 0 && FD_ISSET(sock, &wSet)) {
            int len = strlen(wBuffer);
            int n = write(sock, wBuffer, len);
            if(n == -1) {
                perror("write");
                close(sock);
                return;
            } else {
                if(n < len) {
                    puts("n<len");
                    memmove(wBuffer, wBuffer+n, len-n+1);
                } else {
                    memset(wBuffer, 0, sizeof(wBuffer));
                }
            }
        }

        if(rRes<0 || wRes<0 || r1Res<0) {
            perror("select");
            FD_CLR(sock, &sockSet);
            FD_CLR(STDIN_FILENO, &inSet);
            close(sock);
            return;
        }
    }

}



/*
 * compare with void client_run()
 * add some commanders
 *  $upload [filename]  : upload a file to server.(block)
 *  $ls                 : list files from server.
 *  $download [filename]: download a file from server.(block)
 */
void client_run_cmd(const char* ip, int port) {
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

    char buffer[1024];
    int bufsize = 1024;
    int maxfd = sock, rRes, wRes, r1Res;
    fd_set sockSet, inSet, rSet, wSet, r1Set;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    FD_ZERO(&sockSet);
    FD_ZERO(&inSet);
    FD_SET(sock, &sockSet);
    FD_SET(STDIN_FILENO, &inSet);

    bool cmdflag = false, receiveing = false;
    char* cmdbuf[20];
    int cmdbuflen;
//    int memflen = 1024*1024*4;
//    char memfile[memflen];
    FILE * pfile;
    char rfilename[50];
    char wBuffer[1024];
    while(1) {
        r1Set = inSet;
        rSet = sockSet;
        wSet = sockSet;

        rRes = select(maxfd+1, &rSet, NULL, NULL, &timeout);

        if(rRes > 0 && FD_ISSET(sock, &rSet)) {
            ssize_t n = read(sock, buffer, sizeof(buffer));
            buffer[n] = '\0';
            if(n < 0) {
                perror("read");
                close(sock);
                return;
            } else if(n == 0) {
                printf("Server closed.");
                FD_CLR(sock, &sockSet);
                close(sock);
                return;
            }
            if(!_receFile(pfile, buffer, n, receiveing, rfilename, bufsize)) {
                printf("Message form server:\n %s\n", buffer);
            }
        }

        r1Res = select(maxfd+1, &r1Set, NULL, NULL, &timeout);

        if(r1Res > 0 && FD_ISSET(STDIN_FILENO, &r1Set) && !receiveing) {
            int n = read(STDIN_FILENO, buffer, sizeof(buffer));
            if(n < 0) {
                perror("read");
                FD_CLR(STDIN_FILENO, &inSet);
                close(sock);
                return;
            }
            buffer[n-1] = '\0'; // buffer[n-1] == '\n'
            strcat(wBuffer, buffer);
        }

        wRes = select(maxfd+1, NULL, &wSet, NULL, &timeout);
        if(wRes > 0 && FD_ISSET(sock, &wSet)) {
            int len = strlen(wBuffer);
            int n = write(sock, wBuffer, len);
            if(n == -1) {
                perror("write");
                close(sock);
                return;
            } else {
                if(command(wBuffer, cmdbuf, cmdbuflen)>=0) {
                    if(strcmp("upload", cmdbuf[0]+1) == 0) {
                        if(cmdbuf[1]==NULL || strcmp(cmdbuf[1], "") == 0) {
                            fprintf(stderr, "filename is empty\n");
                        } else {
                            if(!_sendFile(sock, cmdbuf[1])) {
                                fprintf(stderr, "upload fail\n");
                            }
                        }
                    }
                }

                if(n < len) {
                    puts("n<len");
                    memmove(wBuffer, wBuffer+n, len-n+1);
                } else {
                    memset(wBuffer, 0, sizeof(wBuffer));
                }
            }
        }

        if(rRes<0 || wRes<0 || r1Res<0) {
            perror("select");
            FD_CLR(sock, &sockSet);
            FD_CLR(STDIN_FILENO, &inSet);
            close(sock);
            return;
        }
    }
}

#endif //START_CLIENT_H
