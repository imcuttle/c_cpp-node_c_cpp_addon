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
#include <sys/select.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#include "command.h"

int Bind(const char* ip, int port);

void intHandler(int sig) {
    printf("Bye child\n");
    wait(NULL);
    exit(1);
}

void server_run(const char* ip, int port) {

//    signal(SIGCHLD, intHandler);

    int serv_sock = Bind(ip, port);
    if(serv_sock == -1) {
        return;
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
                    buffer[n] = '\0';
                    printf("Message form client %s:%d:\n %s\n",
                           inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port), buffer);
                } else if(n < 0) {
                    perror("recv");
                    close(clnt_sock);
                    break;
                } else {
                    printf("FIN form client %s:%d\n",
                           inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port));
                    close(clnt_sock);
                    break;
                }
            }
        }
    }


    //关闭套接字
    close(serv_sock);
}

int Bind(const char* ip, int port) {
    //创建套接字 (tcp->stream(字节流)   udp->datagram(数据报))
    // http://stackoverflow.com/questions/5815675/strange-thing-sock-dgram-and-sock-stream
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //允许创建端口号相同但IP地址不同的多个socket描述符
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
        return -1;
    } else {
        printf("Success bind Server on %s:%d\n", ip, port);
    }

    return serv_sock;
}


void server_run_select (const char* ip, int port) {

//    signal(SIGCHLD, intHandler);

    int serv_sock = Bind(ip, port);
    if(serv_sock == -1) {
        return;
    }

    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    //接收客户端请求
    sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char str[] = "Hello World!";
    char strip[32];
    char buffer[40];

    // 允许同时打开的文件描述符大小FD_SETSIZE
    int nready, client[FD_SETSIZE], i;
    ssize_t n;
    fd_set rset, allset;

    int maxfd = serv_sock;		/* initialize */
    int maxi = -1;			/* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;	/* -1 indicates available entry */
    FD_ZERO(&allset); //用select函数之前先把集合清零
    //把要检测的 serv_sock，加入到集合里。
    FD_SET(STDIN_FILENO, &allset);
    FD_SET(serv_sock, &allset);

    int connfd, sockfd;

//    printf("allset %p", &allset);
    for ( ; ; ) {
        rset = allset;	/* structure assignment */
//        printf("rset %p", &rset);
        // 返回一个大于0的值，表示有多少文件可读，
        // 如果没有可读的文件，则根据timeout参数再判断是否超时，
        // 若超出timeout的时间，select返回0，若发生错误返回负值。
        // listen rset contains readable
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            close(serv_sock);
            perror("select error");
            return;
        }

        // 监听集合中有变化
        if (nready>0) {
            if(FD_ISSET(serv_sock, &rset)) {
                // printf("%s\n", "check new client connection ");
                /* new client connection */
                clnt_addr_size = sizeof(clnt_addr);
                connfd = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

                write(connfd, str, strlen(str));
                printf("connected from %s at PORT %d\n",
                       inet_ntop(AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip)),
                       ntohs(clnt_addr.sin_port));

                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0) {
                        client[i] = connfd; /* save descriptor */
                        break;
                    }
                if (i == FD_SETSIZE) {
                    close(serv_sock);
                    fputs("too many clients\n", stderr);
                    exit(1);
                }

                FD_SET(connfd, &allset);	/* add new descriptor to set */
                if (connfd > maxfd)
                    maxfd = connfd; /* for select */
                if (i > maxi)
                    maxi = i;	/* max index in client[] array */
                if (--nready == 0)
                    continue;	/* no more readable descriptors */
            }
        }

        if(FD_ISSET(STDIN_FILENO, &rset)) {
            // printf("%s\n", "check new input ");
            char lbuf[1024];
            int n = scanf("%s", lbuf);
            for (i = 0; i <= maxi; i++) {    /* check all clients for data */
                if ((sockfd = client[i]) < 0)
                    continue;
                write(sockfd, lbuf, strlen(lbuf));
            }
        }

        for (i = 0; i <= maxi; i++) {	/* check all clients for data */
            if ( (sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                n = read(sockfd, buffer, sizeof(buffer));
                buffer[n] = '\0';
                if (n == 0) {
                    /* connection closed by client */
                    printf("closed from %s at PORT %d\n",
                           inet_ntop(AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip)),
                           ntohs(clnt_addr.sin_port));
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else if( n > 0 ) {
                    printf("received from %s at PORT %d: %s\n",
                           inet_ntop(AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip)),
                           ntohs(clnt_addr.sin_port), buffer);
                    int j;
                    for (j = 0; j < n; j++)
                        buffer[j] = toupper(buffer[j]);
                    write(sockfd, buffer, n);
                } else {
                    perror("read");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }

                if (--nready == 0)
                    break;	/* no more readable descriptors */
            }


        }
    }


}



void printMultiArr(char** p) {
    while(*p!=NULL) {
        puts(*p++);
    }
}

char* fsize(char *name)
{
    char* tmp = strrchr(name, '/');
    const char* filename = tmp!=NULL? tmp+1: name;
    struct stat stbuf;
    char* buf = (char *) calloc(1024, sizeof(char));
    if (lstat(name, &stbuf) == -1) {
        sprintf(buf, "fsize: can't access %s\n", filename);
        return buf;
    }
    if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
        sprintf(buf, "%8lld   %s\n", stbuf.st_size, filename);
        return buf;
    }
    return NULL;
}

void fsize(char *name, int fileno)
{
    char* filename = strrchr(name, '/')+1;
    struct stat stbuf;
    char* buf = (char *) calloc(1024, sizeof(char));
    if (lstat(name, &stbuf) == -1) {
        sprintf(buf, "fsize: can't access %s\n", filename);
        write(fileno, buf, strlen(buf));
        return;
    }
    if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
        sprintf(buf, "%8lld   %s\n", stbuf.st_size, filename);
        write(fileno, buf, strlen(buf));
    }
}

bool cmd_action(const char* type, char** args, int fileno) {
    if(0 == strcmp(type, "ls")) {
        DIR *dir;
        struct dirent *ent;
        const char* dirname = "data";
        LS:
        if ((dir = opendir (dirname)) != NULL) {
            char s[1024*40] = "\n";
            while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                    continue;
                char b[1024] = "";
                strcat(b, dirname); strcat(b, "/"); strcat(b, ent->d_name);
                char* p = fsize(b);
                strcat(s, p);
                free(p);
            }
            closedir(dir);
            write(fileno, s, strlen(s));
        } else {
            if(errno == ENOENT) {
                mkdir(dirname, 0700);
                goto LS;
            }
            perror ("opendir");
        }
    } else if(0 == strcmp(type, "get")) {
        if(args[0] == NULL || strcmp(trim(args[0]), "")==0) {
            write(fileno, "get what?", 10);
        } else {
            char* rename = args[0];
            if(args[1]!=NULL && strcmp(trim(args[1]), "")!=0) {
                rename = args[1];
            }
            char x[strlen(args[0])+10];
            sprintf(x, "data/%s", args[0]);

            if(!_sendFile(fileno, x, rename)) {
                write(fileno, "down fail", 9);
            }
        }
    } else if(0 == strcmp(type, "quit")) {
        close(fileno);
        return false;
    }
    return true;
}


/*
 * compare with void server_run()
 * add some commanders
 *  $upload [filename]  : upload a file to server.(block)
 *  $ls                 : list files from server.
 *  $download [filename]: download a file from server.(block)
 */

void server_run_cmd(const char* ip, int port) {

    int serv_sock = Bind(ip, port);
    if(serv_sock == -1) {
        return;
    }

    listen(serv_sock, 20);

    sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    char str[] = "Hello World!";
    char strip[32];
    int bufsize = 40;
    char buffer[bufsize], rfilename[50];

    int clnt_sock;

    char* cmdbuf[20];
    int cmdbuflen;

    FILE* pfile;
    bool receiveing = false;


    while((clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_addr_size)) >= 0) {
        write(clnt_sock, str, sizeof(str));
        printf("connected from %s at PORT %d\n",
               inet_ntop(AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip)),
               ntohs(clnt_addr.sin_port));

        pid_t pid = fork();
        if(pid == 0) {
            ssize_t n;
            while(1) {
                n = recv(clnt_sock, buffer, sizeof(buffer), 0);
                if (n > 0){
                    buffer[n] = '\0';
                    if(!_receFile(pfile, buffer, n, receiveing, rfilename, bufsize)) {
                        printf("Message form client %s:%d: \n%s\n", inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port), buffer);
                        if(command(buffer, cmdbuf, cmdbuflen)>=0) {
                            if(!cmd_action(cmdbuf[0]+1, cmdbuf+1, clnt_sock)) {
                                exit(1);
                            }
                        }
                    }
                } else if(n < 0) {
                    perror("recv");
                    close(clnt_sock);
                    break;
                } else {
                    printf("FIN form client %s:%d\n",
                           inet_ntop( AF_INET, &clnt_addr.sin_addr, strip, sizeof(strip) ), ntohs(clnt_addr.sin_port));
                    close(clnt_sock);
                    break;
                }
            }
        }
    }

    wait(NULL);
    //关闭套接字
    close(serv_sock);
}

#endif //START_SERVER_H
