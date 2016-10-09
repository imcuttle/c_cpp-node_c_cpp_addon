//
// Created by Moyu on 16/10/8.
//

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSG_TRY "try again\n"

int main(void)
{
    char buf[10];
    int fd, n;
    // because default STDIN_FILENO is opened
    // method 1, reopen shell
//    fd = open("/dev/tty", O_RDONLY|O_NONBLOCK);
    // method 2, update STDIN_FILENO flags
    int flags;
    flags = fcntl(STDIN_FILENO, F_GETFL);
    flags |= O_NONBLOCK;
    fd = fcntl(STDIN_FILENO, F_SETFL, flags);

    if(fd<0) {
        perror("open /dev/tty");
        exit(1);
    }
    tryagain:
    n = read(fd, buf, 10);
    if (n < 0) {
        if (errno == EWOULDBLOCK) {
            sleep(1);
            write(STDOUT_FILENO, MSG_TRY, strlen(MSG_TRY));
            goto tryagain;
        }
        perror("read /dev/tty");
        exit(1);
    }
    write(STDOUT_FILENO, buf, n);
    close(fd);
    return 0;
}
/*
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buf[10];
    int n;
    //保存在内核的终端设备输入缓冲区, 阻塞
    n = read(STDIN_FILENO, buf, 10);
    if (n < 0) {
        perror("read STDIN_FILENO");
        exit(1);
    }
    write(STDOUT_FILENO, buf, n);
    return 0;
}*/