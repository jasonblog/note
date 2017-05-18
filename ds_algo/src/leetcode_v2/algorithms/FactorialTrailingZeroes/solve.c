#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int trailingZeroes(int n)
{
    if (n < 0) {
        return 0;
    }

    int sum = 0;

    while (n) {
        sum += n / 5;
        n /= 5;
    }

    return sum;
}
int main(int argc, char** argv)
{
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("%d\n", trailingZeroes(n));
    }

    return 0;
}
