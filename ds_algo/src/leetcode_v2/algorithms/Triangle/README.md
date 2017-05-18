## Triangle

Given a triangle, find the minimum path sum from top to bottom. Each step you may move to adjacent numbers on the row below.

For example, given the following triangle

```
[
     [2],
    [3,4],
   [6,5,7],
  [4,1,8,3]
]
```

The minimum path sum from top to bottom is `11` (i.e., `2 + 3 + 5 + 1 = 11`).

Note:
Bonus point if you are able to do this using only O(n) extra space, where n is the total number of rows in the triangle. 

## Solution

典型DP，显然`dp[i][j] = min(dp[i - 1][j], dp[i - 1][j - 1]`, 因此

```cpp
int minimumTotal(vector<vector<int>> &a) {
	int n = a.size();
	if (n < 1)
		return 0;
	vector<vector<int>> dp(n, vector<int>(n, 0));
	dp[0][0] = a[0][0];
	for (int i = 1; i < n; ++i) {
		dp[i][0] = dp[i - 1][0] + a[i][0];
		dp[i][i] = dp[i - 1][i - 1] + a[i][i];
		for (int j = 1; j < i; ++j) {
			dp[i][j] = min(dp[i - 1][j], dp[i - 1][j - 1]) + a[i][j];
		}
	}
	return *min_element(begin(dp[n - 1]), end(dp[n - 1]));
}
```

最后需要O(n)时间寻找最小值，我们可以从底部推顶部，此时`dp[i][j] = min(dp[i + 1][j], dp[i + 1][j + 1]`

```cpp
int minimumTotal(const vector<vector<int>> &a) {
	int n = a.size();
	if (n < 1)
		return 0;
	vector<vector<int>> dp(n, vector<int>(n, 0));
	for (int i = 0; i < a[n - 1].size(); ++i) {
		dp[n - 1][i] = a[n - 1][i];
	}
	for (int i = n - 2; i >= 0; --i) {
		for (int j = 0; j <= i; ++j) {
			dp[i][j] = min(dp[i + 1][j], dp[i + 1][j + 1]) + a[i][j];
		}
	}
	return dp[0][0];
}
```

以上方法都能解决问题，关键是需要O(n<sup>2</sup>)的空间，题目要求O(n)。

从状态转移方程可以看出，`dp[i][j]`只和上一行有关，和其他行无关，我们可以复用这些空间，覆盖原来的值，从而达到压缩空间的效果.

```cpp
int compress_minimumTotal(const vector<vector<int>> &a) {
	int n = a.size();
	if (n < 1)
		return 0;
	vector<int> dp;
	for_each(begin(a[n - 1]), end(a[n - 1]), [&dp](int i){dp.push_back(i);});
	for (int i = n - 2; i >= 0; --i) {
		for (int j = 0; j <= i; ++j) {
			dp[j] = min(dp[j], dp[j + 1]) + a[i][j];
		}
	}
	return dp[0];
}
```


