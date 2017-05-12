#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool isMatch(char* s, char* p)
{
    if (*p == '\0') { // p为空
        return *s == '\0';
    }

    if (*(p + 1) == '\0' || *(p + 1) != '*') {
        if (*s == '\0' || (*p != '.' && *s != *p)) {
            return false;
        }

        return isMatch(s + 1, p + 1);
    }

    int i = -1, len = strlen(s);

    while (i < len && (i < 0 || *p == '.' || *(s + i) == *p)) {
        if (isMatch(s + i + 1, p + 2)) {
            return true;
        }

        i++;
    }

    return false;
}
int main(int argc, char** argv)
{
    char a[20], p[20];

    while (scanf("%s%s", a, p) != EOF) {
        if (isMatch(a, p)) {
            printf("%s %s -> true\n", a, p);
        } else {
            printf("%s %s -> false\n", a, p);
        }
    }

    return 0;
}
