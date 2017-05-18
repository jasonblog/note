#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    char s[100];

    while (scanf("%s", s) != EOF) {
        printf("%d\n", atoi(s));
    }

    return 0;
}
