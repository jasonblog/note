## Search a 2D Matrix

Write an efficient algorithm that searches for a value in an m x n matrix. This matrix has the following properties:

Integers in each row are sorted from left to right.
The first integer of each row is greater than the last integer of the previous row.
For example,

Consider the following matrix:

```
[
  [1,   3,  5,  7],
  [10, 11, 16, 20],
  [23, 30, 34, 50]
]
```

Given `target = 3`, return `true`.

## Solution

題目說明矩陣是有序的，首先我們可以從右上方查找，若target小於當前值，則左移動，否則下移動.

```cpp
bool searchMatrix(const vector<vector<int>> &matrix, int target) {
	if (matrix.empty() || matrix[0].empty())
		return false;
	int n = matrix.size(), m = matrix[0].size();
	int i = 0, j = m - 1;
	while (i < n && j >= 0) {
		if (matrix[i][j] == target)
			return true;
		if (matrix[i][j] > target) {
			j--;
		} else
			i++;
	}
	return false;
}
```

時間複雜度是O(n + m).

是否有更優的算法呢。。顯然可以把二維數組當作一維數組。然後使用二分查找。

```cpp
bool searchMatrix(const vector<vector<int>> &matrix, int target) {
	if (matrix.empty() || matrix[0].empty())
		return false;
	int n = matrix.size(), m = matrix[0].size();
	int s = 0, t = n * m - 1;
	while (s <= t) {
		int mid = s + ((t - s) >> 1);
		int i = mid / m, j = mid % m;
		if (matrix[i][j] == target)
			return true;
		if (matrix[i][j] > target) {
			t = mid - 1;
		} else
			s = mid + 1;
	}
	return false;
}
```

此時時間複雜度優化成O(log(n * m))

但這還不是最優的結果，我們可以使用二分搜索的擴展查找，先定位行，再定位列，初始化s = 0, t = n - 1, n 為行數:

* 令`mid = (s + t) / 2`
* 若target 在第mid行範圍內，即`matrix[mid][0] <= target <= matrix[mid][m - 1]`, 二分搜索第mid行`binarySearch(matrix[mid], target)`
* 若target < `matrix[mid][0]`, 則在上半部分，即`t = mid - 1`;
* 若target > `matrix[mid][m - 1]`, 則在下半部分， 即`s = mid + 1`

```cpp
bool searchMatrix(const vector<vector<int>> &matrix, int target) {
	if (matrix.empty())
		return false;
	int s = 0, t = matrix.size() - 1, n = matrix[0].size();
	while (s <= t) {
		int mid = s + ((t - s) >> 1);
		if (target < matrix[mid][0]) {
			t = mid - 1;
		} else if (target > matrix[mid][n - 1]) {
			s = mid + 1;
		} else {
			return binarySearch(matrix[mid], target);
		}
	}
	return false;
}
```

時間複雜度為O(log(n) + log(m)).
