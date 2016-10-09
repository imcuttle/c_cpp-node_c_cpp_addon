//
// Created by Moyu on 16/10/8.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(void)
{
    char *p;
    int fd = open("./hellp", O_RDWR);
    if (fd < 0) {
        perror("open hellp");
        exit(1);
    }
    p = (char*)mmap(NULL, 6, PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fd);
    puts((char *)p);
    p[0] = 's';
    p[1] = 'a';
//    p[0] = 0x30313233;
    munmap(p, 6);
    return 0;
}