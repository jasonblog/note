#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static inline int MAX(int a, int b)
{
    return a > b ? a : b;
}
int maxSubArray(int* a, int n)
{
    if (a == NULL || n <= 0) {
        return 0;
    }

    int max = a[0], sum = a[0];

    for (int i = 1; i < n; ++i) {
        if (sum < 0) {
            sum = a[i];
        } else {
            sum += a[i];
        }

        max = MAX(max, sum);
    }

    return max;
}
void print(int* a, int n)
{
    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
}
int main(int argc, char** argv)
{
    int a[20], n;
    memset(a, 0, sizeof(a));

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", &a[i]);
        }

        printf("%d\n", maxSubArray(a, n));
    }

    return 0;
}
