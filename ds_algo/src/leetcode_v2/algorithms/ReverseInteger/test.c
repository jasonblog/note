#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
int reverse(int x)
{
    int sign = 0, ans = 0;

    if (x < 0) {
        x = -x;
        sign = 1;
    }

    while (x) {
        ans *= 10;
        ans += x % 10;
        x /= 10;
    }

    return sign ? -ans : ans;
}
int main(int argc, char** argv)
{
    printf("%d\n", INT_MAX);
}
