## Valid Sudoku

Determine if a Sudoku is valid, according to: [Sudoku Puzzles - The Rules](http://sudoku.com.au/TheRules.aspx).

The Sudoku board could be partially filled, where empty cells are filled with the character '.'.


A partially filled sudoku which is valid.

Note:
*A valid Sudoku board (partially filled) is not necessarily solvable. Only the filled cells need to be validated.

Subscribe to see which companies asked this question*

## Solution

此題是純粹的模擬題，只需要根據數獨的規則分別判斷每一行、每一列、每一個九宮格是否出現重複數字即可。判斷數字是否重複使用hash表即可。

### 判斷每一行

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

### 判斷每一列

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

### 判斷每一個九宮格

需要注意確定每一個九宮格對應行首和列首，假設九宮格從0開始編號，則第n個九宮格的行位置為`n / 3 * 3`, 列位置為`n % 3 * 3`

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

實習代碼為：

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
