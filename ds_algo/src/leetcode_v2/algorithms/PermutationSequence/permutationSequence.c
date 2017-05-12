#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* listRemove(char* s, int i)
{
    int n = strlen(s), j;

    for (j = i; j < n - 1; ++j) {
        s[j] = s[j + 1];
    }

    s[j] = 0;
    return s;
}
char* getPermutation(int n, int k)
{
    char* ans = malloc(sizeof(char) * (n + 1));
    char* nums = malloc(sizeof(char) * n);
    const int FAC[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};

    for (int i = 0; i < n; ++i) {
        nums[i] = '0' + i + 1;
    }

    k--;
    int cur = 0;

    for (int i = n - 1; i > 0; --i) {
        int index = k / FAC[i];
        k %= FAC[i];
        ans[cur++] = nums[index];
        listRemove(nums, index);
    }

    ans[cur++] = nums[0];
    ans[cur] = 0;
    free(nums);
    return ans;
}
int main(int argc, char** argv)
{
    for (int i = 1; i <= 6; ++i) {
        printf("%s\n", getPermutation(3, i));
    }

    return 0;
}
