#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int searchInsert(int A[], int n, int target)
{
    if (A == NULL || n < 1) {
        return 0;
    }

    int s = 0, t = n - 1;

    while (s <= t) {
        int mid = (s + t) >> 1;

        if (A[mid] == target) {
            return mid;
        }

        if (A[mid] > target) {
            t = mid - 1;
        } else {
            s = mid + 1;
        }
    }

    return s;
}
int main(int argc, char** argv)
{
    int a[] = {1, 3, 5, 6};
    printf("%d\n", searchIndex(a, 4, 5));
    printf("%d\n", searchIndex(a, 4, 2));
    printf("%d\n", searchIndex(a, 4, 7));
    printf("%d\n", searchIndex(a, 4, 0));
    printf("%d\n", searchIndex(a, 4, 3));
    return 0;
}
