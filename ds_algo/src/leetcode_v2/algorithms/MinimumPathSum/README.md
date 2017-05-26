## Minimum Path Sum

Given a m x n grid filled with non-negative numbers, find a path from top left to bottom right which minimizes the sum of all numbers along its path.

**Note: You can only move either down or right at any point in time.**

## Solution

最開始想到用BFS，這思維！

顯然這是最最經典直接的DP問題。

設`dp[i][j]`表示`a[0][0]`到`a[i][j]`的最小路徑, 則顯然有動態方程`dp[i][j] = MIN(dp[i - 1][j], dp[i][j - 1])`,
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

時間和空間複雜度都是O(nm)。時間複雜度沒有什麼優化方案，當空間可以採用壓縮的方法,顯然`dp[i][j]`只依賴於左邊和上邊一個值,即`dp[i - 1][j]`和`dp[i][j - 1]`,
而與前面的值無關。所以處理第i行j列時，與i-1行的該j列有關，以及i行j-1列有關，我們只需要聲明dp[i], dp表示第i行j列的值,舊值dp[i]相當於`dp[i - 1][j]`,
 `dp[i - 1]`相當於`dp[i][j - 1]`,於是`dp[i] = MIN (dp[i], dp[i - 1]`
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
