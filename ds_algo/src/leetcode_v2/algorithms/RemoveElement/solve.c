#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int removeElement(int a[], int n, int key)
{
    int k = 0;

    for (int i = 0; i < n; ++i) {
        if (a[i] != key) {
            a[k++] = a[i];
        }
    }

    return k;
}
int main(int argc, char** argv)
{
    int a[] = {1, 2, 3, 2, 3, 2, 1, 6, 2};
    int n = removeElement(a, 9, 2);

    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
    return 0;
}
