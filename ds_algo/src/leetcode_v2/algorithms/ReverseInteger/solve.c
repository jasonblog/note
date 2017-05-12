#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int reverse(int x)
{
    int sign = 0, ans = 0;
    const int BOUND = INT_MAX / 10;

    if (x == -2147483648) {
        return 0;
    }

    if (x < 0) {
        x = -x;
        sign = 1;
    }

    while (x) {
        ans *= 10;
        ans += x % 10;
        x /= 10;

        if (ans >= BOUND) {
            break;
        }
    }

    if (x == 0) {
        return sign ? -ans : ans;
    }

    if (ans > BOUND) { // 此时乘以10,无论如何都要溢出
        return 0;
    }

    if (sign &&
        x == 8) { // 刚好是最大的负数，其实这种情况不可能出现
        return -2147483648;
    }

    if (x > 7) { // ans已经等于INT_MAX / 10，如果个位大于7溢出.
        return 0;
    }

    ans *= 10;
    ans += x; // 此时一定是2147483641
    return sign ? -ans : ans;
}
int main(int argc, char** argv)
{
    int x;
    printf("%d\n", INT_MAX);

    while (scanf("%d", &x) != EOF) {
        printf("%d\n", reverse(x));
    }

    return 0;
}
