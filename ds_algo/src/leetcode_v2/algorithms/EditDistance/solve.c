#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static inline int min2(int a, int b)
{
    return a < b ? a : b;
}
static inline int min3(int a, int b, int c)
{
    return min2(min2(a, b), c);
}
int minDistance(char* w1, char* w2)
{
    if (w1 == NULL || *w1 == 0) {
        if (w2 == NULL || *w2 == 0) {
            return 0;
        } else {
            return strlen(w2);
        }
    }

    if (w2 == NULL || *w2 == 0) {
        return strlen(w1);
    }

    const int len1 = strlen(w1);
    const int len2 = strlen(w2);
    int dp[len1 + 1][len2 + 1];
    dp[0][0] = 0;

    for (int i = 1; i <= len1; ++i) {
        dp[i][0] = i;
    }

    for (int j = 1; j <= len2; ++j) {
        dp[0][j] = j;
    }

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int case1 = dp[i - 1][j] + 1; // 删除当前字符
            int case2 = dp[i][j - 1] + 1;
            int case3 = dp[i - 1][j - 1] + (w1[i - 1] == w2[j - 1] ? 0 : 1);
            dp[i][j] = min3(case1, case2, case3);
        }
    }

    return dp[len1][len2];
}
int main(int argc, char** argv)
{
    char s1[20], s2[20];

    while (scanf("%s%s", s1, s2) != EOF) {
        printf("|%s, %s| = %d\n", s1, s2, minDistance(s1, s2));
    }

    return 0;
}
