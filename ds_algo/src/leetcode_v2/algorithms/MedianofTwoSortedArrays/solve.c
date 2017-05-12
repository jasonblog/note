#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
static inline int min(int a, int b)
{
    return a < b ? a : b;
}
int getkth(int a[], int n, int b[], int m, int k)
{
    if (k <= 0 || k > n + m) {
        return -1;
    }

    if (n > m) {
        return getkth(b, m, a, n, k);
    }

    if (n == 0) {
        return b[k - 1];
    }

    if (k == 1) {
        return min(a[0], b[0]);
    }

    int i = min(n, k / 2), j = min(m, k / 2);

    if (a[i - 1] > b[j - 1]) {
        return getkth(a, n, b + j, m - j, k - j);
    } else {
        return getkth(a + i, n - i, b, m, k - i);
    }
}
double findMedianSortedArrays(int* a, int n, int* b, int m)
{
    if ((n + m) & 0x1) {
        return getkth(a, n, b, m, ((n + m) >> 1) + 1);
    }

    int left = getkth(a, n, b, m, (n + m) >> 1);
    int right = getkth(a, n, b, m, ((n + m) >> 1) + 1);
    return (left + right) / 2.0;
}
double getMedian(int* a, int n, int* b, int m)
{
    return findMedianSortedArrays(a, n, b, m);
}
int main(int argc, char** argv)
{
    int a[] = {1, 3, 5, 7, 9};
    int b[] = {2, 4, 6, 8, 10};
    printf("%lf\n", getMedian(a, 5, b, 5));
    return 0;
}
