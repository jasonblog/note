#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int numTrees(int n)
{
    int dp[n + 1];
    memset(dp, 0, sizeof(dp));
    dp[0] = 1;
    dp[1] = 1;

    for (int i = 2; i <= n; ++i) {
        for (int j = 1; j <= i; ++j) {
            dp[i] += dp[j - 1] * dp[i - j];
        }
    }

    return dp[n];
}
int main(int argc, char** argv)
{
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("numTrees(%d) = %d\n", n, numTrees(n));
    }

    return 0;
}
