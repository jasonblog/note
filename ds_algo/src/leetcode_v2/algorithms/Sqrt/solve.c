#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

int mySqrt1(int x)
{
    if (x == 0) {
        return 0;
    }

    int left = 1, right = x;
    int ans = 0;;

    while (left < right) {
        //int mid = (left >> 1) + (right >> 1); 1/2 + 1/2 = 0
        //int mid = (left + right) >> 1; overflow
        //printf("left = %d, right = %d\n", left, right);
        int mid = left + ((right - left) >> 1);

        if (mid == left || mid == x / mid) {
            return mid;
        }

        if (mid < x / mid) {
            left = mid;
        } else {
            right = mid;
        }

    }

    return left;
}
int mySqrt2(int x)
{
    long ans = 0;
    long bit = 1L << 16;

    while (bit) {
        ans |= bit;

        if (ans * ans > x) {
            ans ^= bit;
        }

        bit >>= 1;
    }

    return ans;
}
int mySqrt3(int x)
{
    if (x == 0) {
        return 0;
    }

    int i;

    for (i = 1; i < x / 2; ++i)
        if (i > x / i) {
            break;
        }

    return i - 1;
}
bool test(int x)
{
    int r1 = mySqrt1(x);
    int r2 = mySqrt2(x);
    int r3 = mySqrt3(x);
    printf("test %d\n", x);

    if (r1 != r3) {
        printf("%d->%d : %d\n", x, r1, r3);
        return false;
    }

    return true;
}
int main(int argc, char** argv)
{
    for (int i = 0; i <= INT_MAX; ++i) {
        test(i);
    }

    return 0;
}
