/*
http://ubuntuforums.org/archive/index.php/t-225713.html
*/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>


int getch(void)
{
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
    newt = oldt; /* copy old settings to new settings */
//    newt.c_lflag &= ~(ICANON | ISIG);
//    newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */

    ch = getchar(); /* standard getchar call */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
    return ch; /*return received char */
}

struct xyz {
    char x;
    char y;
    char z;
    bool isNormal() {
        return x != 27;
    }
    bool isArrow() {
        return x==27 && y==91;
    }
} xyz;

struct xyz getkxyz() {
    int x = ' ';
    int y = ' ';
    int z = ' ';
    x = getch();

    if (x == 27) {
        y = getch();
        z = getch();
    } else {

//        putchar(x);
    }

    struct xyz o;
    o.x = x;
    o.y = y;
    o.z = z;
    return o;
}