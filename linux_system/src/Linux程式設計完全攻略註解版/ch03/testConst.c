#include <stdio.h>
char* a = "hello";
int main(void)
{
    char* b = "hello";
    printf("in main:");

    if (a == b) {
        printf("yes,a==b\n");
    } else {
        printf("no,a!=b\n");
    }

    printf("in sub:");
    test();
    return 0;
}

