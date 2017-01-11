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

#define MAXLINE 80
#define SERV_PORT 8000

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr, from_addr;
    int sockfd, n;
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

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (n == -1) {
            perror("sendTO");
            return 0;
        }
        printf("n: %d\n", n);
        memset(recvbuf, 0, strlen(recvbuf));
        socklen_t len = (socklen_t)sizeof(from_addr);
        n = recvfrom(sockfd, (void *)recvbuf, (size_t)MAXLINE, 0, (struct sockaddr *)&from_addr, &len);
        if (n == -1) {
            perror("recvfrom");
            return 0;
        }
        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &from_addr.sin_addr, str, sizeof(str)),
               ntohs(from_addr.sin_port));

        write(STDOUT_FILENO, recvbuf, n);
    }

    close(sockfd);
    return 0;
}