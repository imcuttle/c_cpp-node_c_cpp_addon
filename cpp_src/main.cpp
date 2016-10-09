#include <iostream>
#include <string>

#define _Bnd(X, bnd) (sizeof (X) + (bnd) & ~(bnd))

char* _strcpy (char* dest, char* src);

int main() {
    std::cout << sizeof("Hello, World!") << " " << std::endl;
    char str[] = "Hello, World!";
    char *pStr = str;
    std::cout << strlen(str) << " " << sizeof(str) << " " << sizeof(char*) << std::endl;
    std::cout << &pStr << " " << sizeof(pStr) << " " << strlen(pStr) << std::endl;

//    char *iptStr = new char[20];
//    scanf("%s", iptStr);
//    printf("%s\n", iptStr);

    char s[] = "2323";
    char* dest;

    char* newdest = _strcpy(dest, s);

//     print address of char array
    printf("%s\n", &dest[0]);
    printf("%s\n", &newdest[0]);

    return 0;
}

char* _strcpy (char* dest, char* src) {
    char *t = dest;
    while ((*dest++ = *src++) != '\0');
    return t;
}