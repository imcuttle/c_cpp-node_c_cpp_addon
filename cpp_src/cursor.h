//
// Created by Moyu on 16/10/11.
//

#ifndef START_CURSOR_H
#define START_CURSOR_H

#include <stdio.h>
#include <unistd.h>

// 清除屏幕
#define CLEAR() printf("\033[2J")

// 上移光标
#define MOVEUP(x) printf("\033[%dA", (x))

// 下移光标
#define MOVEDOWN(x) printf("\033[%dB", (x))

// 左移光标
#define MOVELEFT(y) printf("\033[%dD", (y))

// 右移光标
#define MOVERIGHT(y) printf("\033[%dC",(y))

// 定位光标
#define MOVETO(x,y) printf("\033[%d;%dH", (x), (y))

// 光标复位
#define RESET_CURSOR() printf("\033[H")

// 隐藏光标
#define HIDE_CURSOR() printf("\033[?25l")

// 显示光标
#define SHOW_CURSOR() printf("\033[?25h")

//反显
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\x1B[0m"


#define P_RESET(clr, x) printf(clr, x, RESET);

#define P_BLU_A(x) printf(KBLU, x);

#define P_RED(x) P_RESET(KRED, x);
#define P_NRM(x) P_RESET(KNRM, x);
#define P_GRN(x) P_RESET(KGRN, x);
#define P_YEL(x) P_RESET(KYEL, x);
#define P_BLU(x) P_RESET(KBLU, x);
#define P_MAG(x) P_RESET(KMAG, x);
#define P_CYN(x) P_RESET(KCNY, x);
#define P_WHT(x) P_RESET(KWHT, x);


#endif //START_CURSOR_H
