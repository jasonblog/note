## Number of Islands 

Given a 2d grid map of '1's (land) and '0's (water), count the number of islands. An island is surrounded by water and is formed by connecting adjacent lands horizontally or vertically. You may assume all four edges of the grid are all surrounded by water.
```
Example 1:

11110
11010
11000
00000
Answer: 1

Example 2:

11000
11000
00100
00011
Answer: 3
```

## Solution

当遇到1时，把所有相邻的1置0,利用搜索思想.

最开始想到BFS，内存溢出，改成DFS采用递归的方式AC

## Code
```cpp
int numIslands(vector<vector<char>> &grid) {
	int result = 0;
	int n = grid.size();
	if (n == 0)
		return 0;
	int m = grid[0].size();
	for (auto i = 0; i < n; ++i) {
		for (auto j = 0; j < m; ++j) {
			if (grid[i][j] == '1') {
				result++;
				clear(grid, i, j);
			}
		}
	}
	return result;
}
void clear(vector<vector<char>> &grid, int i, int j) {
	int n = grid.size();
	int m = grid[0].size();
	if (i < 0 || j < 0 || i >= n || j >= m || grid[i][j] == '0')
		return;
	grid[i][j] = '0';
	clear(grid, i - 1, j);
	clear(grid, i + 1, j);
	clear(grid, i, j - 1);
	clear(grid, i, j + 1);
}
```
