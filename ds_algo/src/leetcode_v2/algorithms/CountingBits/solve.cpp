#include <stdio.h>
#include <stdlib.h>
class Solution
{
public:
    vector<int> countBits(int num)
    {
        vector<int> result(num + 1, 0);

        for (int i = 1; i <= num; ++i) {
            result[i] = result[i >> 1] + (i & 1);
        }

        return result;
    }
};
int countBit(int n)
{
    int sum = 0;

    while (n) {
        sum++;
        n &= (n - 1);
    }

    return sum;
}
int* countBits_bad(int num, int* size)
{
    *size = num + 1;
    int* ans = (int*)malloc(sizeof(int) * *size);

    for (int i = 0; i <= num; ++i) {
        ans[i] = countBit(i);
    }

    return ans;
}
int* countBits(int num, int* size)
{
    *size = num + 1;
    int* ans = (int*)malloc(sizeof(int) * *size);

    for (int i = 1; i <= num; ++i) {
        ans[i] = ans[i >> 1] + (i & 1);
    }

    return ans;
}
int main(int argc, char** argv)
{
    int n;

    while (scanf("%d", &n) != EOF) {
        int size = 0;
        int* ans = countBits(n, &size);

        for (int i = 0; i < size; ++i) {
            printf("%d ", ans[i]);
        }

        printf("\n");
        free(ans);
    }

    return 0;
}
