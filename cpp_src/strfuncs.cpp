//
// Created by Moyu on 16/10/8.
//

#include <stdio.h>
#include <string.h>
#include <iostream>

int main(void) {
    //memset
    //由malloc分配的内存初值也是不确定的，也可以用memset清零
//    char* str;
//    str = (char *) memset(str, 98, 10);
//    puts(str);

    //strlen
//    char buf[5] = "niho";
//    std::clog << strlen(buf) << std::endl;

    //memcpy & memmove & strdup
//    char buf2[20] = "hello world\n";
//    std::cout << sizeof(buf2);
//    memcpy(buf2 + 1, buf2, strlen(buf2));
//    printf(buf2);
//    memmove(buf2 + 1, buf2, strlen(buf2));
//    printf(buf2);
//    if(strdup(buf2) == buf2) {
//        puts("equal");
//    } else {
//        puts("not equal");
//    }

    //strcat
    char d[10] = "foo";
    char s[10] = "bar";
    strcat(d, s);
    printf("%s %s\n", d, s);

    char** x;
//    printf("%p", *x);
    x = (char **) malloc(sizeof(char**));
    printf("%p\n", *x);
    *x = "abc";
//    *(x+1) = "abc";
//    *x[0] = 'x';
    printf("%s\n", *(x+1));
    printf("%s\n", *x);


    return 0;
}