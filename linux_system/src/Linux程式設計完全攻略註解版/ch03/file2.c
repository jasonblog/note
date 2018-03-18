#include<stdio.h>
extern char* a;
void test(void)
{
    char* c = "hello";

    if (a == c) {
        printf("yes,a==c\n");
    } else {
        printf("no,a!=c\n");
    }
}

