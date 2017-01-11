//
// Created by Moyu on 2017/1/10.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

struct From {
    sockaddr_in addr;
    int socket;
};

void * thread_main (void* args) {
    From* from = (From *) args;
    sockaddr_in from_addr = from->addr;
    int socket = from->socket;
    u_char buf[1024];
    char str[INET_ADDRSTRLEN];
    int n = read(socket, buf, sizeof(buf));
    if(n==0) {
        close(socket);
        return NULL;
    }
    printf("from %d %s:%d => %s\n",
           socket,
           inet_ntop(AF_INET, &from_addr.sin_addr, str, sizeof(str)), ntohs(from_addr.sin_port),
           buf
    );
    write(socket, "你好", strlen("你好")+1);
    while (1);
    close(socket);
    return NULL;
}

int main () {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in local_addr, from_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(6788);

    if( 0 > bind(s, (sockaddr *)&local_addr, sizeof(local_addr))) {
        perror("bind");
        return 1;
    }
    if ( 0> listen(s, 20)) {
        perror("listen");
        return 1;
    }

    int c;
    int len = sizeof(from_addr);
    while ((c = accept(s, (sockaddr*)&from_addr, (socklen_t*)&len))) {
        pthread_t th;
        From from;
        memcpy(&from.addr, &from_addr, sizeof(from_addr));
        from.socket = c;
        pthread_create(&th, NULL, thread_main, &from);
    }
    return 0;
}