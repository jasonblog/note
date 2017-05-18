#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int singleNumber(int a[], int n)
{
    int result = 0;

    for (int i = 0; i < n; ++i) {
        result ^= a[i];
    }

    return result;
}
int main(int argc, char** argv)
{
    int a[] = {1, 1, 2, 3, 3, 4, 5, 4, 5};
    printf("%d\n", singleNumber(a, 9));
    return 0;
}
