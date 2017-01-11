//
// Created by Moyu on 2017/1/2.
//


#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>



void printTime (char* id) {
    time_t currtime = time(NULL);
    printf("%sID: %s, TIME: %s%s", "\x1B[32m", id, ctime(&currtime), "\x1B[0m");
}

#define BUFLEN 255
#define LOOP 10

int main () {
    pid_t pid1 = fork();
    pid_t pid2 = -1;
    if(pid1!=0) {
        pid2 = fork();
    }
    int times = LOOP;
    char buf[BUFLEN+1];

    if (pid1 == 0) {
    // child process 1
        while (times-->0) {
            snprintf(buf, BUFLEN, "PROCESS1::LOOP %d", LOOP-times);
            printTime(buf);
            sleep(1);
        }
    }
    if(pid2 == 0) {
    // child process 2
        while (times-->0) {
            snprintf(buf, BUFLEN, "PROCESS2::LOOP %d", LOOP-times);
            printTime(buf);
            sleep(1);
        }
    }
    // 主进程退出，所有子进程都退出
    wait(NULL);
//    kill(2);

    return 0;
}