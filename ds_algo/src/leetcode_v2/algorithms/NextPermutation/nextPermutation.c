#include <stdio.h>
#include <stdlib.h>
void swap(int* a, int* b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
void reverse(int* a, int s, int t)
{
    int i = s, j = t;

    while (i < j) {
        swap(&a[i], &a[j]);
        ++i, --j;
    }
}
void nextPermutation(int num[], int n)
{
    int i, j;

    for (i = n - 2; i >= 0 && num[i] >= num[i + 1]; --i);

    if (i < 0) {
        reverse(num, 0, n - 1);
        return;
    }

    for (j = n - 1; j > i && num[j] <= num[i]; --j);

    swap(&num[i], &num[j]);
    reverse(num, i + 1, n - 1);
}
void print(int* a, int n)
{
    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
}
int main(int argc, char** argv)
{
    int d[] = {1, 1};
    nextPermutation(d, 2);
    print(d, 2);
    return 0;
}
