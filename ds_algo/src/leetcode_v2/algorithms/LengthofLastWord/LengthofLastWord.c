#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int lengthOfLastWord(const char* s)
{
    if (!s || !*s) {
        return 0;
    }

    int len = strlen(s);
    int sum = 0;
    int i = len - 1;

    while (i >= 0 && !isalpha(s[i])) {
        i--;
    }

    while (isalpha(s[i])) {
        ++sum;
        i--;
    }

    return sum;
}
int main(int argc, char** argv)
{
    const char* p;
    p = "hello world";
    printf("%s, %d\n", p, lengthOfLastWord(p));
    p = "a";
    printf("%s, %d\n", p, lengthOfLastWord(p));
    p = "              ";
    printf("%s, %d\n", p, lengthOfLastWord(p));

    if (argc > 1) {
        p = argv[1];
        printf("%s, %d\n", p, lengthOfLastWord(p));
    }
}
