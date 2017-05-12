#include <stdio.h>
#include <stdlib.h>
static inline int max(int a, int b)
{
    return a >= b ? a : b;
}
static inline int min(int a, int b)
{
    return a <= b ? a : b;
}
int get(int i)
{
    return i & 0x1;
}
int robWithoutCircle(int num[], int n)
{
    int df[2];
    int i, result = -1;

    if (n <= 0) {
        return 0;
    }

    if (n == 1) {
        return num[0];
    }

    df[0] = num[0];
    df[1] = max(df[0], num[1]);
    result = max(df[0], df[1]);

    for (i = 2; i < n; ++i) {
        df[get(i)] = max(df[get(i - 2)] + num[i], df[get(i - 1)]);
        result = max(result, df[get(i)]);
    }

    return result;
}
int rob(int* nums, int n)
{
    if (nums == NULL || n <= 0) {
        return 0;
    }

    if (n == 1) {
        return nums[0];
    }

    return max(robWithoutCircle(nums, n - 1), robWithoutCircle(nums + 1, n - 1));
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
    int a[20], n;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", &a[i]);
        }

        print(a, n);
        printf("result = %d\n", rob(a, n));
    }

    return 0;
}
