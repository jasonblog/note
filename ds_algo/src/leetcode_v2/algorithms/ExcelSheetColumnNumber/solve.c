#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int titleToNumber(char* s)
{
    if (s == NULL) {
        return 0;
    }

    int result = 0;

    while (*s) {
        result *= 26;
        result += *s - 'A' + 1;
        s++;
    }

    return result;
}
int main(int argc, char** argv)
{
    char buf[15];

    while (scanf("%s", buf) != EOF) {
        printf("%s -> %d\n", buf, titleToNumber(buf));
    }

    return 0;
}
