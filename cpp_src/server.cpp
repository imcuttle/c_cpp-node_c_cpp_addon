//
// Created by Moyu on 16/10/7.
//
#include "server.h"


int main(int argc, char* args[]) {

    char *ip = "127.0.0.1";
    int port = 3234;
    if(argc > 1) {
        ip = args[1];
        if(argc > 2) {
            port = atoi(args[2]);
        }
    }

    server_run(ip, port);

    return 0;
}