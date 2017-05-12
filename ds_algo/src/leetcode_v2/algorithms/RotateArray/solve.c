#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void swap(int* a, int* b)
{
    if (a == NULL || b == NULL || a == b) {
        return;
    }

    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
void reverse(int a[], int s, int t)
{
    int i = s, j = t;

    while (i < j) {
        swap(&a[i++], &a[j--]);
    }
}
void rotate(int a[], int n, int k)
{
    k %= n;
    reverse(a, 0, n - 1);
    reverse(a, 0, k - 1);
    reverse(a, k, n - 1);
}
int main(int argc, char** argv)
{
    int a[] = {1, 2, 3, 4, 5, 6, 7};
    rotate(a, 7, 3);

    for (int i = 0; i < 7; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
    return 0;
}
