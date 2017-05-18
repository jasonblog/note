#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int maxProfit(int* a, int n)
{
    if (a == NULL || n < 1) {
        return 0;
    }

    int profit = 0;

    for (int i = 1; i < n; ++i) {
        int diff = a[i] - a[i - 1];

        if (diff > 0) {
            profit += diff;
        }
    }

    return profit;
}
int main(int argc, char** argv)
{
    int a[] = {3, 1, 2, 6, 5, 6};
    printf("%d\n", maxProfit(a, 6));
    return 0;
}
