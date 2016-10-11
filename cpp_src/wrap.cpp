//
// Created by Moyu on 16/10/11.
//

/* wrapper.c */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    if (argc != 2) {
        fputs("usage: wrapper file\n", stderr);
        exit(1);
    }
    fd = open(argv[1], O_RDONLY);
    if(fd<0) {
        perror("open");
        exit(1);
    }
    int fds[2];
    pipe(fds);

    if(fork() == 0) {
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        close(fds[1]);
        close(fd);
        execlp("cat", "cat", NULL);
        perror("exec cat");
    } else {
        dup2(fds[1], STDOUT_FILENO);
        write(STDOUT_FILENO, "cnmm\n", 6);
        write(STDOUT_FILENO, "cnmmxx\n", 8);
        close(fds[0]);
        close(fds[1]);
//        close(fd);
    }
    wait(NULL);
    exit(1);
}