#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int getSign(long a, long b)
{
    if ((a < 0 && b > 0) || (a > 0 && b < 0)) {
        return 1;
    }

    return 0;
}
long labs(long a)
{
    return a >= 0 ? a : -a;
}
int divide(int dividend, int divisor)
{
    if (divisor == 0) {  // 除数为0
        return dividend >= 0 ? INT_MAX : INT_MIN;
    }

    long n = dividend; // 防止溢出，当其中一个=INT_MIN
    long m = divisor;
    int sign = getSign(n, m); // 保存结果的符号
    n = labs(n);
    m = labs(m);
    long ans = 0;
    long a; // 防止 a << 1溢出
    long k;

    while (n >= m) {
        a = m, k = 1;

        while (n > (a << 1)) {
            a <<= 1;
            k <<= 1;
        }

        n -= a;
        ans += k;
    }

    ans = sign ? -ans : ans;

    if (ans > INT_MAX) {
        return INT_MAX;
    }

    if (ans < INT_MIN) {
        return INT_MIN;
    }

    return ans;
}
int main(int argc, char** argv)
{
    int a, b;

    while (scanf("%d%d", &a, &b) != EOF) {
        printf("%d\n", divide(a, b));
    }

    return 0;
}
