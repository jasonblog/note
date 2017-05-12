#include <stdio.h>
#include <stdlib.h>
int max(int a, int b)
{
    return a >= b ? a : b;
}
int rob(int num[], int n)
{
    int df[2];
    int i, result = -1;

    if (n == 1) {
        return num[0];
    }

    df[0] = num[0];
    df[1] = max(df[0], num[1]);
    result = max(df[0], df[1]);

    for (i = 2; i < n; ++i) {
        df[i % 2] = max(df[(i - 2) % 2] + num[i], df[(i - 1) % 2]);
        result = max(result, df[i % 2]);
    }

    return result;
}
int main(int argc, char** argv)
{
    int num[] = {2, 1, 1, 2};
    printf("%d\n", rob(num, 4));
    return 0;
}
