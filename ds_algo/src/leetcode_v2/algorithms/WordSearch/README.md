## Word Search

Given a 2D board and a word, find if the word exists in the grid.

The word can be constructed from letters of sequentially adjacent cell, where "adjacent" cells are those horizontally or vertically neighboring. The same letter cell may not be used more than once.

For example,

```
Given board =

[
  ["ABCE"],
  ["SFCS"],
  ["ADEE"]
]
```

```
word = "ABCCED", -> returns true,
word = "SEE", -> returns true,
word = "ABCB", -> returns false.
```

## Solution

典型的DFS，定義dfs方法，當前位置為`<i,j>`, 當前字符指針p:

* 若p已經被訪問， 返回false
* 若`board[i][j] != *p`, 返回false
* 否則若p是最後一個字符
* 訪問dfs(i - 1,j, p + 1), dfs(i + 1,j, p + 1), dfs(i, j - 1, p + 1), dfs(i, j + 1, p + 1), 若其中一個返回true，則返回true

```cpp
bool dfs(const vector<vector<char>> &a, const char *word, int i, int j) {
	if (a[i][j] != *word || visited[i][j]) // 當前字符不相等，直接false
		return false;
	if (*(word + 1) == 0) // 這已經是最後一個字符，無需搜索
		return true;
	visited[i][j] = true; // 標識為已經訪問
	int n = a.size(), m = a[0].size();
	if (i - 1 >= 0) {
		if (dfs(a, word + 1, i - 1, j))
			return true;
	}
	if (i + 1 < n) {
		if (dfs(a, word + 1, i + 1, j))
			return true;
	}
	if (j - 1 >= 0) {
		if (dfs(a, word + 1, i, j - 1))
			return true;
	}
	if (j + 1 < m) {
		if (dfs(a, word + 1, i , j + 1))
			return true;
	}
	visited[i][j] = false; // 回溯, 重新設置該字符未訪問標識
	return false;
}
```

## 擴展

[Word Search II](../WordSearchII):查詢多個單詞是否
