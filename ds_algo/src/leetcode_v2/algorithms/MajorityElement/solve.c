#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int majorityElement(int a[], int n)
{
    if (n < 2) {
        return a[0];
    }

    int result = a[0];
    int count = 0;

    for (int i = 0; i < n; ++i) {
        if (a[i] == result) {
            ++count;
        } else {
            --count;
        }

        if (count == 0) {
            count = 1;
            result = a[i];
        }
    }

    return result;
}
int main(int argc, char** argv)
{
    int a[20];
    int n;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        printf("%d\n", majorityElement(a, n));
    }

    return 0;
}
