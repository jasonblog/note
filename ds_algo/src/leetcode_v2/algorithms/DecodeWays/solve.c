#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static inline int isValid(char a, char b)
{
    if (a == '1' || (a == '2' && b < '7')) {
        return 1;
    }

    return 0;
}
int numDecodings(char* s)
{
    int n;

    if (s == NULL || (n = strlen(s)) < 1) {
        return 0;
    }

    int sum = 0;

    if (s[0] != '0' && s[1] != '0') {
        sum = 1;
    }

    for (int i = 1; i < n; ++i) {
        if (s[i] != '0' && s[i + 1] != '0') {
            sum++;
        }

        sum += isValid(s[i - 1], s[i]);
    }

    return sum;
}
int main(int argc, char** argv)
{
    char buf[20];

    while (scanf("%s", buf) != EOF) {
        printf("%d\n", numDecodings(buf));
    }

    return 0;
}
