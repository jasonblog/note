#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static inline int min(int a, int b)
{
    return a < b ? a : b;
}
int getkth(int s[], int m, int l[], int n, int k)
{
    if (m > n) {
        return getkth(l, n, s, m, k);
    }

    if (m == 0) {
        return l[k - 1];
    }

    if (k == 1) {
        return min(s[0], l[0]);
    }

    int i = min(m, k / 2), j = min(n, k / 2);

    if (s[i - 1] > l[j - 1]) {
        return getkth(s, m, l + j, n - j, k - j);
    } else {
        return getkth(s + i, m - i, l, n, k - i);
    }
}
double getMedia(int a[], int m, int b[], int n)
{
    if ((m + n) & 0x1) {
        return getkth(a, m, b, n, ((m + n) >> 1) + 1);
    } else {
        return (getkth(a, m, b, n, (m + n) >> 1) + getkth(a, m, b, n,
                ((m + n) >> 1) + 1)) / 2.0;
    }
}
int main(int argc, char** argv)
{
    int a[] = {1, 3, 5, 7, 9};
    int b[] = {2, 4, 6};
    printf("%d\n", getkth(a, 5, b, 3, 7));
    return 0;
}
