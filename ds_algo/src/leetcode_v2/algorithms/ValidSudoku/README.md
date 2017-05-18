## Valid Sudoku

Determine if a Sudoku is valid, according to: [Sudoku Puzzles - The Rules](http://sudoku.com.au/TheRules.aspx).

The Sudoku board could be partially filled, where empty cells are filled with the character '.'.


A partially filled sudoku which is valid.

Note:
*A valid Sudoku board (partially filled) is not necessarily solvable. Only the filled cells need to be validated.

Subscribe to see which companies asked this question*

## Solution

此题是纯粹的模拟题，只需要根据数独的规则分别判断每一行、每一列、每一个九宫格是否出现重复数字即可。判断数字是否重复使用hash表即可。

### 判断每一行

```cpp
bool check_row(const vector<vector<char>> &a, int row) {
	vector<bool> used(9, false);
	for (char i : a[row]) {
		if ('.' == i)
			continue;
		int pos = i - '0' - 1;
		if (used[pos])
			return false;
		else
			used[pos] = true;
	}
	return true;
}
```

### 判断每一列

```cpp
bool check_col(const vector<vector<char>> &a, int col) {
	vector<bool> used(9, false);
	for (int i = 0; i < 9; ++i) {
		if (a[i][col] == '.')
			continue;
		int pos = a[i][col] - '0' - 1;
		if (used[pos])
			return false;
		else
			used[pos] = true;
	}
	return true;
}
```

### 判断每一个九宫格

需要注意确定每一个九宫格对应行首和列首，假设九宫格从0开始编号，则第n个九宫格的行位置为`n / 3 * 3`, 列位置为`n % 3 * 3`

```cpp
bool check_box(const vector<vector<char>> &a, int box) {
	int row_begin = box / 3 * 3;
	int col_begin = box % 3 * 3;
	vector<bool> used(9, false);
	for (int i = row_begin; i < row_begin + 3; ++i) {
		for (int j = col_begin; j < col_begin + 3; ++j) {
			if (a[i][j] == '.')
				continue;
			int pos = a[i][j] - '0' - 1;
			if (used[pos])
				return false;
			else
				used[pos] = true;
		}
	}
	return true;
}
```

实习代码为：

```cpp
bool isValidSudoku(vector<vector<char>>& board) {
	for (int i = 0; i < 9; ++i) {
		if (!(check_row(board, i)
		    && (check_col(board, i))
		    && (check_box(board, i)))
		)
			return false;
	}
	return true;
}
```
