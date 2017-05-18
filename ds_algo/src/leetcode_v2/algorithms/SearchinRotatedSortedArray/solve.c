#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int binarySearch(int* a, int s, int t, int key)
{
    while (s <= t) {
        int mid = s + ((t - s) >> 1);

        if (a[mid] == key) {
            return mid;
        }

        if (a[mid] > key) {
            t = mid - 1;
        } else {
            s = mid + 1;
        }
    }

    return -1;
}
int search(int* a, int n, int key)
{
    int s = 0, t = n - 1;

    while (s <= t) {
        if (a[s] < a[t]) {
            return binarySearch(a, s, t, key);
        }

        int mid = s + ((t - s) >> 1);

        //printf("s = %d, t = %d, mid = %d\n", s, t, mid);
        if (a[mid] == key) {
            return mid;
        }

        if (a[mid] < a[t]) {
            if (a[mid] < key && key <= a[t]) {
                return binarySearch(a, mid + 1, t, key);
            } else {
                t = mid - 1;
            }
        } else { // a[mid] > a[t]
            if (a[s] <= key && key < a[mid]) { // 若key 在s和mid之间
                return binarySearch(a, s, mid - 1, key);
            } else {
                s = mid + 1;
            }
        }
    }

    return -1;
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
    int a[] = {4, 5, 6, 7, 8, 1, 2, 3};
    print(a, 8);
    printf("%d:%d\n", 8, search(a, 8, 8));
    return 0;
}
