#include <stdio.h>
#include <stdlib.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b));
#include <assert.h>
#include <limits.h>
int rangeBitwiseAnd(int m, int n)
{
    if (m == n) {
        return m;
    }

    if (m == 0) {
        return 0;
    }

    int base = 1;
    int t = n;

    while ((t >> 1)) {
        base <<= 1;
        t >>= 1;
    }

    int ans = 0;

    while (base) {
        int a = base & m;
        int b = base & n;

        if (a != b) {
            return ans;
        }

        ans |= a;
        base >>= 1;
    }

    return ans;
}
int slowRangeBitwiseAnd(int m, int n)
{
    int ans = n;

    for (int i = n - 1; i >= m && ans; --i) {
        ans &= i;
    }

    return ans;
}
int midRangeBitwiseAnd(int m, int n)
{
    int base = 1;
    int t = n;

    while ((t >> 1)) {
        base <<= 1;
        t >>= 1;
    }

    if (m < base) {
        return 0;
    }

    int ans = n;

    for (int i = n - 1; i >= m; --i) {
        ans &= i;
    }

    return ans;
}
int fastRangeBitwiseAnd(int m, int n)
{
    return rangeBitwiseAnd(m, n);
}
int slow(int m, int n)
{
    return slowRangeBitwiseAnd(m, n);
}
int mid(int m, int n)
{
    return midRangeBitwiseAnd(m, n);
}

int fast(int m, int n)
{
    return fastRangeBitwiseAnd(m, n);
}
int test(int i, int j)
{
    int s = slow(i, j);
    int m = mid(i, j);
    int f = fast(i, j);

    if (s != f) {
        printf("[%d, %d], slow = %d, fast = %d\n", i, j, s, f);
        return 1;
    }

    if (s != m) {
        printf("[%d, %d], slow = %d, mid = %d\n", i, j, s, m);
        return 1;
    }

    return 0;
}
int main(int argc, char** argv)
{
    for (int i = 0; i < 200; i ++)
        for (int j = i; j < 200; j++)
            if (test(i, j)) {
                return 1;
            }

    return 0;
}
