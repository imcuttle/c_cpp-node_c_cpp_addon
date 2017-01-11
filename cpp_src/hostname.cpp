//
// Created by Moyu on 16/10/9.
//

#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

int main(int argc, char **argv) {
    char name[100];
    gethostname(name, sizeof(name));
    printf("your pc is %s\n", name);

    char   *ptr="baidu.com", **pptr;
    struct hostent *hptr;



    if(argc>1) {
        ptr = argv[1];
    }

    if((hptr = gethostbyname(ptr)) == NULL) {
        perror("gethostbyname error");
        return 0;
    }

    printf("official hostname: %s\n", hptr->h_name);
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++) {
        printf(" alias: %s\n", *pptr);
    }

    char   str[32];

    switch(hptr->h_addrtype) {
        case AF_INET:
        case AF_INET6:
            for(pptr=hptr->h_addr_list; *pptr!=NULL; pptr++)
                printf(" address: %s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
            break;
        default:
            printf("unknown address type\n");
            break;
    }


    return 0;
}