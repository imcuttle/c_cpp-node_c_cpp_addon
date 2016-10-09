//
// Created by Moyu on 16/10/8.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <zconf.h>

void _perrorno(const char *s) {
    char* s1 = strerror(errno);
    char* out = new char[strlen(s1) + strlen(s) + 4];

    strcat(out, s);
    strcat(out, " : ");
    strcat(out, s1);
    strcat(out, "\0");

    fputs(out, stderr);
}

int main () {

//    int arr[] = {13, 3223, 233};
//    printf("%d", sizeof(arr));
    FILE *fp;

    if ( (fp = fopen("file.txt", "a+")) == NULL) {
//        perror("Error open file /tmp/file1");
        _perrorno("Error open file ./file.txt");
        exit(1);
    }
    fflush(fp);

    while (1) {
        sleep(1);
        time_t rawtime;
        time( &rawtime );
        fprintf(fp, "time: %s", ctime(&rawtime));
        fprintf(stdout, "time: %s", ctime(&rawtime));
        fflush(fp);
    }

    fclose(fp);
    return 0;
}