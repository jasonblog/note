## Unique Binary Search Trees
Given n, how many structurally unique BST's (binary search trees) that store values 1...n?

For example,

Given n = 3, there are a total of 5 unique BST's.
```
   1         3     3      2      1
    \       /     /      / \      \
     3     2     1      1   3      2
    /     /       \                 \
   2     1         2                 3
```

## Solution

首先n為0時，樹的個數為0，即空樹，n為1時，樹的個數為1，即單節點樹

假設根節點固定，則`以該根組成的樹的個數 = 左子樹個數 * 右子樹個數`。

當數組為 1, 2, 3, ..., n 時,基於以下原則的構建的BST樹具有唯一性:以i為根節點的樹,其左子樹由 [1, i-1] 構成,其右子樹由 [i+1, n] 構成。即以i為根節點的樹的總數等於numTrees(i - 1) * numTrees(n - i)

因此要求f(n),只要迭代即可:

```c
for (int i = 2; i <= n; ++i) {
	for (int j = 1; j <= i; ++j) { /* 分別以j為根 */
		f[i] += f[j - 1] * f[i - j];
	}
}
```

## Code
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
