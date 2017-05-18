#include <stdio.h>
int removeDuplicates(int a[], int n)
{
    int i, j;

    if (n == 0) {
        return 0;
    }

    for (i = 1, j = 0; i < n; ++i) {
        if (a[i] != a[i - 1]) {
            a[++j] = a[i];
        }
    }

    return j + 1;
}
void print(int a[], int n)
{
    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
}
int main(int argc, char** argv)
{
    int a[] = {1, 1, 1, 2, 2, 3};
    int n = removeDuplicates(a, 0);
    print(a, n);
    return 0;
}
