#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

const double ERROR = 1e-8;
int doubleEQ(double x, double y)
{
    return fabs(x - y) < ERROR;
}
double myPow(double x, int n)
{
    if (n == 0) {
        return 1;
    }

    if (n == 1) {
        return x;
    }

    if (n == -1) {
        if (fabs(x - 0) < ERROR) { // x位0 1/0 没有意义
            return INT_MAX;
        }

        return 1.0 / x;
    }

    if (doubleEQ(x, 0)) {
        return 0;
    }

    if (doubleEQ(x, 1)) {
        return 1;
    }

    if (doubleEQ(x, -1)) {
        if ((n & 0x1)) {
            return -1;
        } else {
            return 1;
        }
    }

    if ((n & 0x1)) {
        int mid = (n - 1) / 2;
        double half = myPow(x, mid);
        return half * half * x;
    } else {
        double half = myPow(x, n / 2);
        return half * half;
    }
}
int main(int argc, char** argv)
{
    double x;
    int n;

    while (scanf("%lf%d", &x, &n)) {
        printf("%lf\n", myPow(x, n));
    }

    return 0;
}
