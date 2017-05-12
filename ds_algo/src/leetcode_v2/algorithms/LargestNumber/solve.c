#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define N 100 * 100
bool isDup(char* s1, char* s2)
{
    if (s1 == NULL) {
        return s2 == NULL;
    }

    int len1 = strlen(s1), len2 = strlen(s2);

    if (len1 == 0 || len2 == 0) {
        return false;
    }

    char* p1, *p2;

    if (len1 >= len2) {
        if (len1 % len2 != 0) {
            return false;
        }

        p1 = s1;
        p2 = s2;
    } else {
        if (len2 % len1 != 0) {
            return false;
        }

        p1 = s2;
        p2 = s1;
    }

    while (*p1) {
        if (*p1 != *p2) {
            return false;
        }

        *p1++, *p2++;

        if (*p2 == 0) {
            if (len1 >= len2) {
                p2 = s2;
            } else {
                p2 = s1;
            }
        }
    }

    return true;
}
int cmp(const void* a, const void* b)
{
    char s1[20], s2[20];
    sprintf(s1, "%d", *(int*)a);
    sprintf(s2, "%d", *(int*)b);

    if (isDup(s1, s2)) {
        return 0;
    }

    char* p1 = s1, *p2 = s2;

    while (*p1 && p2) {
        if (*p1 != *p2) {
            return *p2 - *p1;
        }

        p1++, p2++;

        if (*p1 == 0) {
            p1 = s1;
        }

        if (*p2 == 0) {
            p2 = s2;
        }
    }

    return 0;
}
char* largestNumber(int* num, int n)
{
    char* result = malloc(sizeof(char) * N);
    memset(result, 0, sizeof(char) * N);
    qsort(num, n, sizeof(num[0]), cmp);

    if (num[0] == 0) {
        strcpy(result, "0\0");
        return result;
    }

    int sum = 0;

    for (int i = 0; i < n; ++i) {
        sum += sprintf(result + sum, "%d", num[i]);
    }

    return result;
}
int main(int argc, char** argv)
{
    int i = 123, j = 456;
    cmp2(&i, &j);
    int a[] = {33, 30, 332, 5, 9};
    int b[] = {0, 0, 0, 0};
    int c[] = {824, 938, 1399, 5607, 6973, 5703, 9609, 4398, 8247};
    int d[] = {121, 12};
    int e[] = {3, 43, 48, 94, 85, 33, 64, 32, 63, 66};
    printf("%s\n", largestNumber(a, 5));
    printf("%s\n", largestNumber(b, 4));
    printf("%d\n", strcmp(largestNumber(c, 9),
                          "9609938824824769735703560743981399"));
    printf("%s\n", largestNumber(d, 2));
    printf("%s\n", largestNumber(e, 10));
    return 0;
}
