#include <stdio.h>
char* a = "hello";
void test(void)
{
    char* c = "hello";

    if (a == c) {
        printf("yes,a==c\n");
    } else {
        printf("no,a!=c\n");
    }
}

int main(void)
{
    char* b = "hello";
    char* d = "hello1";

    if (a == b) {
        printf("yes,a==b\n");
    } else {
        printf("no,a!=b\n");
    }

    test();

    if (a == d) {
        printf("yes,a==d\n");
    } else {
        printf("no,a!=d\n");
    }

    return 0;
}

