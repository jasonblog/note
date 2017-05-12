#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
static inline int MIN(int a, int b)
{
    return a < b ? a : b;
}
static inline int MAX(int a, int b)
{
    return a > b ? a : b;
}
int maxProfit(int* a, int n)
{
    int max = a[n - 1];
    int profit = 0;

    for (int i = n - 2; i >= 0; --i) {
        max = MAX(max, a[i]);
        profit = MAX(profit, max - a[i]);
    }

    return profit;
}
/*
int maxProfit(int *a, int n)
{
    int min = a[0];
    int max = 0;
    for (int i = 1; i < n; ++i) {
        min = MIN(min, a[i]);
        max = MAX(max, a[i] - min);
    }
    return max;
}
*/
int main(int argc, char** argv)
{
    int a[20], n;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        printf("maxProfit = %d\n", maxProfit(a, n));
    }

    return 0;
}
