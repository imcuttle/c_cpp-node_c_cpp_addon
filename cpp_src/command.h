//
// Created by Moyu on 16/10/18.
//

#ifndef START_COMMAND_H
#define START_COMMAND_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <string.h>


char* fileTail(const char*, int);
bool isfileHead(const char *);

int command (char* s, char** buf, int &n) {
    if(s==NULL || s[0]!='$' || s[1]== ' ') {
        return -1;
    }
    char* saveptr = NULL;
    int i=0;

    while((buf[i++] = strtok_r(s, " \t\r\n", &saveptr)) != NULL) {
        s = NULL;
    }
    n = i;
    return i;
}

bool _receFile(FILE* &pfile, char* buffer, ssize_t n, bool& receiveing, char* rfilename, int size) {
    bool run = false;
    char* last = NULL;
    if(!receiveing && isfileHead(buffer)) {
        memset(rfilename, 0, 50);
        strcpy(rfilename, "data/");
        if (stat(rfilename, NULL) == -1) {
            mkdir(rfilename, 0700);
        }

        char name[40];
        sscanf(buffer, "---file: %s\r\n", name);
        int othlen = 11+strlen(name);
        int addonlen = n-othlen;

        strcat(rfilename, name);
        pfile = fopen(rfilename, "wb+");

        receiveing = true;
        printf("Downloading %s ...\nhead addon: %s\n\n",
               rfilename, buffer+othlen);
        if(addonlen > 0) {
            fwrite(buffer+othlen, addonlen, 1, pfile);
        }
        run= true;
    }
    if(receiveing && (last = fileTail(buffer, n))!=NULL) {
        receiveing = false;
        fwrite(buffer, last-buffer, 1, pfile);
        fclose(pfile);
        printf("Downloaded %s. and saved\n", rfilename);
        run= true;
    } else if(receiveing && !run) {
        printf("download chunk, size: %ld\n", n);
        if(n<size) {
            receiveing = false;
            fclose(pfile);
        }
        fwrite(buffer, n, 1, pfile);
        run= true;
    }
    return run;
}

bool _sendFile(int out_fd, const char* file,  const char* rename) {
    int fd = open(file, O_RDONLY);
//    char* tmp = strrchr(file, '/');
//    const char* filename = tmp!=NULL? tmp+1: file;
    if(fd==-1) {
        char b[1024];
        sprintf(b, "open failed %s", file);
        perror(b);
        return false;
    }
    struct stat state;
    fstat(fd, &state);

    printf("sending File %s ...\n", file);
    off_t offset = 0;
    off_t len = 0;

    char head[1024], sizehd[1024];
    sprintf(head, "---file: %s\r\n", rename);
//    sprintf(sizehd, "---size: %lld\r\n\r\n", state.st_size);

    struct sf_hdtr hdtr;
    iovec headers, trailers;
    headers.iov_base = head;
    headers.iov_len = strlen(head);
//    trailers.iov_base = (void *)"file---\r\n"; //todo: don't recv sometimes ??
//    trailers.iov_len = 9;
    hdtr.headers = &headers;
    hdtr.hdr_cnt = 1;
    hdtr.trailers = NULL;
    hdtr.trl_cnt = 0;

    if(0 == sendfile(fd, out_fd, offset, &len, &hdtr, 0)) {
        close(fd);
        write(out_fd, "file---\r\n", 9);
        printf("sendFile %s success, return len: %lld.\n", file, len);
        return true;
    } else {
        close(fd);
        write(out_fd, "file---\r\n", 9);
        perror("sendfile");
        return false;
    }
}

char *end_strstr(const char *haystack, int n, const char *needle)
{
    int needlen = strlen(needle), hlen = n;
    if(needlen > hlen)
        return NULL;
    char* p = (char*) &haystack[hlen-needlen];
    return 0 == strcmp(p, needle) ? p: NULL;
}

bool startsWith(const char* s, const char* chunk) {
    return strstr(s, chunk) == s;
}

bool isfileHead(const char* s) {
    return startsWith(s, "---file: ");
}

bool endsWith(const char* s, int n, const char* chunk) {
    return end_strstr(s, n, chunk) == s+n-strlen(chunk);
}

char* fileTail(const char*s, int n) {
    return endsWith(s, n, "file---\r\n") ? end_strstr(s, n, "file---\r\n"): NULL;
}


char* trim(char *s) {
    if(s==NULL)
        return s;

    int len = strlen(s), i=0, j = len-1;
    for (; i < len; ++i) {
        if(s[i]!=' ')
            break;
    }
    for (; j >= i; j--) {
        if(s[j]!=' ')
            break;
    }
    char* buf = (char*)calloc(j-i+2, sizeof(char));
    for(int k=0; i<j+1; k++, i++) {
        buf[k] = s[i];
    }
    return buf;
}



#endif //START_COMMAND_H
