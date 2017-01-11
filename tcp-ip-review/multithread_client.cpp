//
// Created by Moyu on 2017/1/10.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>



int main () {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in server_addr, from_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6788);

    if( 0 > connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        perror("connect");
        return 1;
    }

    if( 0 > write(s, "client", 6)) {
        perror("write");
        return 1;
    }

    u_char buf[1024];
    int n = read(s, buf, sizeof(buf));
    if(n==0) {
        printf("n==0\n");
        close(s);
        return 1;
    }
    printf("Server: %s\n", buf);
    close(s);
//    while (1);
    return 0;
}