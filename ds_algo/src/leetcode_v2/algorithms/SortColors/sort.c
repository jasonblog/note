#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

void swap(int* a, int* b)
{
    if (a == NULL || b == NULL || a == b) {
        return;
    }

    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
void sortColors(int a[], int n)
{
    if (a == NULL || n == 1) {
        return;
    }

    int zero = 0, two = n - 1;

    for (int i = 0; i <= two; ++i) {
        if (a[i] == 0) {
            swap(&a[i], &a[zero++]);
        }

        if (a[i] == 2) {
            swap(&a[i--], &a[two--]);
        }
    }
}
void print(int a[], int n)
{
    assert(a != NULL);
    assert(n > 0);

    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }

    printf("\n");
}
int main(int argc, char** argv)
{
    int a[10] = {0, 2, 1, 2, 2};
    print(a, 5);
    sortColors(a, 5);
    print(a, 5);
    return 0;
}
