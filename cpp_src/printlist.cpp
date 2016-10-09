//
// Created by Moyu on 16/10/8.
//

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


void printlist(char* begin, ...)
{
    va_list ap;
    char *p;
    char* ps;

    va_start(ap, begin);

    p = va_arg(ap, char *);
    ps = strstr(begin, "%s");
    while (p != NULL && ps != NULL) {
        fputs(strncpy(new char, begin, (size_t)(ps-begin)), stdout);
        fputs(p, stdout);

        begin = ps + strlen("%s");
        p = va_arg(ap, char*);
        ps = strstr(begin, "%s");
    }
    va_end(ap);
}

int main(void)
{
//    printf("%s", "x", "xxx");
    printlist("%s232%s", "hello", "world", "foo", "bar", NULL);
    return 0;
}