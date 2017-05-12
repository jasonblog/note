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

首先n为0时，树的个数为0，即空树，n为1时，树的个数为1，即单节点树

假设根节点固定，则`以该根组成的树的个数 = 左子树个数 * 右子树个数`。

当数组为 1, 2, 3, ..., n 时,基于以下原则的构建的BST树具有唯一性:以i为根节点的树,其左子树由 [1, i-1] 构成,其右子树由 [i+1, n] 构成。即以i为根节点的树的总数等于numTrees(i - 1) * numTrees(n - i)

因此要求f(n),只要迭代即可:

```c
for (int i = 2; i <= n; ++i) {
	for (int j = 1; j <= i; ++j) { /* 分别以j为根 */
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
