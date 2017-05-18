#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
int compareVersion(char* version1, char* version2)
{
    char* p = version1;
    char* q = version2;

    while (*p || *q) {
        int d1 = 0;

        while (*p && isdigit(*p)) {
            d1 *= 10;
            d1 += (*p - '0');
            p++;
        }

        int d2 = 0;

        while (*q && isdigit(*q)) {
            d2 *= 10;
            d2 += (*q - '0');
            q++;
        }

        if (d1 > d2) {
            return 1;
        }

        if (d1 < d2) {
            return -1;
        }

        if (*p) {
            p++;
        }

        if (*q) {
            q++;
        }
    }

    return 0;
}
int main(int argc, char** argv)
{
    char s1[20], s2[20];

    while (scanf("%s%s", s1, s2) != EOF) {
        switch (compareVersion(s1, s2)) {
        case 1:
            printf("%s > %s\n", s1, s2);
            break;

        case -1:
            printf("%s < %s\n", s1, s2);
            break;

        default:
            printf("%s == %s\n", s1, s2);
        }
    }

    return 0;
}
