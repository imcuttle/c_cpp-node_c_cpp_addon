//
// Created by Moyu on 2017/1/2.
//


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
//#include <errno.h>
#include <zconf.h>


void printTime (char* id) {
    time_t currtime = time(NULL);
    printf("%sID: %s, TIME: %s%s", "\x1B[32m", id, ctime(&currtime), "\x1B[0m");
}
void* thread_main(void* pid);

#define BUFLEN 255
#define LOOP 30


int count = 0;
pthread_mutex_t count_mutex;

int main () {
    pthread_mutex_init(&count_mutex, NULL);
    int thread_num = 5;
    pthread_t threads[thread_num];

    for (int i = 0; i < thread_num; ++i) {
        if( 0 != pthread_create(&threads[i], NULL, thread_main, (void*)i)) {
            perror("pthread_create");
            return -1;
        }
    }

    for (int j = 0; j < thread_num; ++j) {
        if (pthread_join(threads[thread_num-j-1], NULL) != 0) {
            return -1;
        }
    }

    printf("Main Process\n");

    return 0;
}




void* thread_main(void* pid) {
    int id = (int) (intptr_t) pid;
    char buf[BUFLEN];
    int times = LOOP;
    while (times-->0) {
        pthread_mutex_lock(&count_mutex);
        snprintf(buf, BUFLEN, "%d::LOOP%d, COUNT: %d", id, LOOP - times, count++);
        printTime(buf);
        pthread_mutex_unlock(&count_mutex);
//        sleep(1);
    }
    return NULL;
}
