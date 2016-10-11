//
// Created by Moyu on 16/10/10.
//

#ifndef START_SHELL_H
#define START_SHELL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fcntl.h>
#include <signal.h>

#include "cursor.h"
#include "keyarrow.h"

using namespace std;

class Shell {
private:
    char buf[1024];
    char** argsBuffer = (char **) calloc(10, sizeof(*argsBuffer));

    char* _start() {
        cout << STR_HEAD;
        int bufi = 0;
        while(1) {
            struct xyz o = getkxyz();
            if(o.isNormal()) {
                if(o.x == '\n')
                    break;
                buf[bufi++]=o.x;
            } else if(o.isArrow()) {
                /*switch(o.z) {
                    case 65:
                        continue;
                    case 66:
                        continue;
                    case 67:
                        MOVERIGHT(1);
                    case 68:
                        MOVELEFT(1);
                }*/
            }
        }
        buf[bufi] = '\0';
        return buf;
    }
    char** split(char *s, const char *delim) {
        if(s== NULL || delim==NULL) {
            return NULL;
        }
        char** tmp = argsBuffer;

        while(1) {
            char* p = strstr(s, delim);
            char* p2 = strstr(s, "|");
            char* p3 = strstr(s, ">");
            if(p != NULL || p2!=NULL || p3!=NULL) {
                int delLen = strlen(delim);
                if((p2 != NULL && (p2<p || p==NULL)) || (p3!=NULL && (p3<p || p==NULL))) {
                    p = p2!=NULL?p2:p3;
                    delLen = 1;
                }
                char* subs = (char *) malloc(p - s + 1);
                strncpy(subs, s, p-s);
                subs[p-s] = '\0';

                if(strcmp(subs, "") != 0) {
                    *tmp++ = subs;
                }
                if(p==p2) {
                    *tmp++ = "|";
                } else if(p==p3) {
                    if(*p3+1=='>') {
                        *tmp++ = ">>";
                        delLen = 2;
                    } else {
                        *tmp++ = ">";
                    }
                }
                s = p + delLen;
            } else {
                *tmp++ = s;
                *tmp = NULL;
                return argsBuffer;
            }
        }
    }

    void FreeArgsBuf() {
        free(argsBuffer);
    }
    void printStrArr(char** ss) {
        while(*ss!=NULL) {
            cout << *ss++ << endl;
        }
    }
    bool isSp(char *s) {
        while(*s!=NULL) {
            if(*s++ != ' ') {
                return false;
            }
        }
        return true;
    }
protected:
    const char* STR_HEAD = "$mShell> ";
    static const int HISTORY_LEN = 1000;
    char* history[HISTORY_LEN];
    int hisMax = 0;
    int cpid;
public:
    void killCpid() {
        if(cpid!=-1)
            kill(cpid, SIGKILL);
    }
    bool HistoryContains(char *s) {
        for(int i=0; i<hisMax; i++) {
            if(strcmp(s, history[i])) {
                return true;
            }
        }
        return false;
    }

    Shell() {

    }
    void Run() {

        while(1) {
            char* s = _start();
            if(s == NULL || isSp(s)) {
                continue;
            }

            /*if(!HistoryContains(s)) {
                history[hisMax++] = s;
            }*/

            char** ss = GetArgs(s);
            if(strcmp(ss[0], "cd") == 0) {
                chdir(ss[1]);
                continue;
            }
            if(strcmp(ss[0], "exit") == 0) {
                break;
            }

            char** t = ss;
            bool isPipe = false, isLg= false;
//            printStrArr(t);
            while(*t != NULL) {
                char* p = strstr(*t, "|");
                if(p != NULL) {
                    isPipe = true;
                    break;
                }
                p = strstr(*t, ">");
                if(p != NULL) {
                    isLg = true;
                    break;
                }
                *t++;
            }
            if(isPipe) {
                *t=NULL;
                Fork_Execute_Pipe(ss, t+1);
            } else if(isLg) {
                bool isAppend = false;
                char* path;
                if(strcmp(*(t+1), ">") == 0) {
                    isAppend = true;
                    *(t++)=NULL;
                }
                *t=NULL;
                Fork_Execute_Redirect(isLg, isAppend, ss, *(t+1));
            } else
                Fork_Execute(ss);

        }
    }
    ~Shell(){
        FreeArgsBuf();
    }
    char** GetArgs(char* str) {
        return split(str, " ");
    }
    void gotoxy(int x, int y) {
        printf("\033[%d;%dH", x, y);
    }
    bool Fork_Execute_Redirect(bool isLg, bool isAppend, char* lefts[], char* path) {
        int flags = O_WRONLY|O_CREAT;
        if(isAppend) {
            flags|=O_APPEND;
        } else {
            flags|=O_TRUNC;
        }
        int fd = open(path, flags, S_IRWXU);
        if(fd==-1) {
            perror("open");
            return false;
        }
        if((cpid = fork()) == 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
            execvp(lefts[0], lefts);
            perror("execvp");
            exit(1);
        }
        wait(NULL);
        return true;
    }
    bool Fork_Execute_Pipe(char* args1[], char* args2[]) {
        int fd[2];
        if(-1 == pipe(fd)) {
            perror("pipe");
            return false;
        }
        int pid1 = fork(),pid2 = -1;
        if(pid1 == 0) {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            if(-1 == execvp(args1[0], args1)) {
                cerr << strerror(errno) << endl;
            }
            exit(1);
        } else {
            pid2 = fork();
            if(pid2 == 0) {
    //            int x = open("tmp", O_RDWR|O_CREAT);
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                close(fd[0]);
                if(-1 == execvp(args2[0], args2)) {
                    cerr << strerror(errno) << endl;
                }
                exit(1);
            }
        }
        if(pid1>0&&pid2>0) {
            close(fd[0]);
            close(fd[1]);
        }

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        return true;
    }

    bool Fork_Execute(char* args[]) {
        if(*args==NULL) {
            cerr << "args[0] is NULL." << endl;
            return false;
        }
        int pid = fork();
        cpid = pid;
        if(pid == 0) {
            if(-1 == execvp(args[0], args)){
                cerr << strerror(errno) << endl;
            }
            exit(1);
        }
        wait(NULL);
        return true;
    }

};



#endif //START_SHELL_H
