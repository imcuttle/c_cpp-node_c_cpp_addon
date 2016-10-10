//
// Created by Moyu on 16/10/10.
//

#ifndef START_SHELL_H
#define START_SHELL_H

#include <stdio.h>

void args_analy(int argc, char* args[]) {
    printf("%d\n", argc);
    while(*args != NULL) {
        printf("%s\n", *args++);
    }
}


#endif //START_SHELL_H
