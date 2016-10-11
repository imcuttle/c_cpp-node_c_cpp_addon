//
// Created by Moyu on 16/10/10.
//

#include "shell.h"

Shell shell;
//void signalHandler(int dig) {
//    printf("singal %d", dig);
//    shell.killCpid();
//}
int main (int n, char** args) {
//    signal(SIGINT, (void (*)(int))signalHandler);
    shell.Run();
}