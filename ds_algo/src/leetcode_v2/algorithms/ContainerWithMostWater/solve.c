#include <stdio.h>
#include <stdlib.h>
int maxArea(int a[], int n)
{
    int max = -1;
    int left = 0, right = n - 1;

    for (int i = 1; i < n; ++i) {
        int area = (a[left] <= a[right] ? a[left] : a[right]) * (right - left);
        max = max < area ? area : max;

        if (a[left] <= a[right]) {
            int l = a[left];
            left++;

            while (left < right && a[left] <= l) {
                left++;
            }
        } else {
            int r = a[right];
            right--;

            while (left < right && a[right] <= r) {
                right--;
            }
        }
    }

    return max;
}
int main(int argc, char** argv)
{
    int a[] = {2, 3, 4, 1};
    int b[] = {1, 2, 1};
    printf("%d\n", maxArea(a, 4));
    printf("%d\n", maxArea(b, 3));
    return 0;
}
