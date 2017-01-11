//
// Created by Moyu on 16/11/2.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>

struct A{
    long a;
    int b;
    short c;
    char d[100];
    double e;

    void print() {
        printf("[A] a: %ld, b: %d, c: %d, d: %s, e: %lf\n", this->a, this->b, this->c, this->d, this->e);
    }
};

#define SERV_PORT 8000

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr, from_addr;
    int sockfd, n;
    int MAXLINE = sizeof(struct A);

    char buf[] = "hello world!";
    char recvbuf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    socklen_t servaddr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bzero(&servaddr, sizeof(servaddr));
    bzero(&from_addr, sizeof(from_addr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    struct A a;
    a.a = 123456789;
    a.b = 12345;
    a.c = 123;
    a.e = 123.123;

    while (fgets(a.d, sizeof(a.d), stdin) != NULL) {
        n = sendto(sockfd, (void*)&a, MAXLINE, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (n == -1) {
            perror("sendTO");
            return 0;
        }

        memset(recvbuf, 0, sizeof(recvbuf));
        socklen_t len = (socklen_t) sizeof(from_addr);
        n = recvfrom(sockfd, (void *)recvbuf, (size_t)MAXLINE, 0, (struct sockaddr *)&from_addr, &len);
        if (n == -1) {
            perror("recvfrom");
            return 0;
        }
        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &from_addr.sin_addr, str, sizeof(str)),
               ntohs(from_addr.sin_port));

        struct A* tmp = (struct A *) recvbuf;
//        memcpy(&tmp, recvbuf, MAXLINE);

        tmp->print();
    }

    close(sockfd);
    return 0;
}