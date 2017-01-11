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

struct A {
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

int main(void) {

    int server_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), n, i;
    struct sockaddr_in server_addr, cliaddr;
    int MAXLINE = sizeof(struct A);

    char buf[MAXLINE], str[32];

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    if(-1 == bind(server_sock, (const sockaddr *)&server_addr, sizeof(server_addr))) {
        perror("BIND");
        return 0;
    }

    printf("Accepting connections ...\n");

    while(1) {
        int cliaddr_len = sizeof(cliaddr);
        n = recvfrom(server_sock, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, (socklen_t *)&cliaddr_len);
        if (n == -1) {
            perror("recvfrom error");
            return 0;
        }
        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
               ntohs(cliaddr.sin_port));

        struct A* a;
        memcpy(a, buf, MAXLINE);
        a->print();

        for (i = 0; i < strlen(a->d); i++)
            a->d[i] = toupper(a->d[i]);
        printf("toUpper after\n");
        a->print();
        n = sendto(server_sock, (void *)a, MAXLINE, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
        if (n == -1) {
            perror("sendto");
            return 0;
        }

    }

    return 0;
}