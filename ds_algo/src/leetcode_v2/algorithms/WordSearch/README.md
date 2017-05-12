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

典型的DFS，定义dfs方法，当前位置为`<i,j>`, 当前字符指针p:

* 若p已经被访问， 返回false
* 若`board[i][j] != *p`, 返回false
* 否则若p是最后一个字符
* 访问dfs(i - 1,j, p + 1), dfs(i + 1,j, p + 1), dfs(i, j - 1, p + 1), dfs(i, j + 1, p + 1), 若其中一个返回true，则返回true

```cpp
bool dfs(const vector<vector<char>> &a, const char *word, int i, int j) {
	if (a[i][j] != *word || visited[i][j]) // 当前字符不相等，直接false
		return false;
	if (*(word + 1) == 0) // 这已经是最后一个字符，无需搜索
		return true;
	visited[i][j] = true; // 标识为已经访问
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
	visited[i][j] = false; // 回溯, 重新设置该字符未访问标识
	return false;
}
```

## 扩展

[Word Search II](../WordSearchII):查询多个单词是否
