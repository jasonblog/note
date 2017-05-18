## Minimum Path Sum

Given a m x n grid filled with non-negative numbers, find a path from top left to bottom right which minimizes the sum of all numbers along its path.

**Note: You can only move either down or right at any point in time.**

## Solution

最开始想到用BFS，这思维！

显然这是最最经典直接的DP问题。

设`dp[i][j]`表示`a[0][0]`到`a[i][j]`的最小路径, 则显然有动态方程`dp[i][j] = MIN(dp[i - 1][j], dp[i][j - 1])`,
```cpp
int minPathSum(vector<vector<int>> &grid) {
	if (grid.size() < 1)
		return 0;
	int n = grid.size();
	int m = grid[0].size();
	vector<vector<int>> dp(n, vector<int>(m, 0));
	dp[0][0] = grid[0][0];
	for (int i = 1; i < n; ++i)
		dp[i][0] = dp[i - 1][0] + grid[i][0];
	for (int j = 1; j < m; ++j)
		dp[0][j] = dp[0][j - 1] + grid[0][j];
	for (int i = 1; i < n; ++i)
		for (int j = 1; j < m; ++j)
			dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + grid[i][j];
	return dp[n - 1][m - 1];
}
```

时间和空间复杂度都是O(nm)。时间复杂度没有什么优化方案，当空间可以采用压缩的方法,显然`dp[i][j]`只依赖于左边和上边一个值,即`dp[i - 1][j]`和`dp[i][j - 1]`,
而与前面的值无关。所以处理第i行j列时，与i-1行的该j列有关，以及i行j-1列有关，我们只需要声明dp[i], dp表示第i行j列的值,旧值dp[i]相当于`dp[i - 1][j]`,
 `dp[i - 1]`相当于`dp[i][j - 1]`,于是`dp[i] = MIN (dp[i], dp[i - 1]`
```cpp
int minPathSum(vector<vector<int>> &grid) {
	if (grid.size() < 1)
		return 0;
	int n = grid.size();
	int m = grid[0].size();
	vector<int> dp(n, 0);
	dp[0] = grid[0][0];
	for (int i = 1; i < n; ++i)
		dp[i] = dp[i - 1] + grid[i][0];
	for (int i = 1; i < n; ++i)
		for (int j = 1; j < n; ++j) {
			dp[j] = min(dp[j], dp[j - 1]) + grid[i][j];
		}
	return dp[n - 1];
}
```
