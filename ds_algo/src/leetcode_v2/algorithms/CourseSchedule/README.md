## Course Schedule

There are a total of n courses you have to take, labeled from 0 to n - 1.

Some courses may have prerequisites, for example to take course 0 you have to first take course 1, which is expressed as a pair: [0,1]

Given the total number of courses and a list of prerequisite pairs, is it possible for you to finish all courses?

For example:

```
2, [[1,0]]
```

There are a total of 2 courses to take. To take course 1 you should have finished course 0. So it is possible.

```
2, [[1,0],[0,1]]
```

There are a total of 2 courses to take. To take course 1 you should have finished course 0, and to take course 0 you should also have finished course 1. So it is impossible.

Note:
The input prerequisites is a graph represented by a list of edges, not adjacency matrices. Read more about how a graph is represented.

Hints:

* This problem is equivalent to finding if a cycle exists in a directed graph. If a cycle exists, no topological ordering exists and therefore it will be impossible to take all courses.
* Topological Sort via DFS - A great video tutorial (21 minutes) on Coursera explaining the basic concepts of Topological Sort.
* Topological sort could also be done via BFS.

## Solution

顯然這是一道拓撲排序題。求拓撲排序的算法過程:

* 求出所有節點的入度`in-degree`
* 搜索`in-degree`，找出入度為0的節點，若沒有找到入讀為0的節點，則一定存在環，返回false，否則，從節點中刪除該節點，並與之相關聯的所有有向邊。
* 重複以上，直到所有的節點訪問完畢，返回true

首先求出所有的入度(注意題目的圖是後面指向前面):

```cpp
vector<int> degree(n, 0);
for (auto p : request) {
	//g[p.second][p.first] = 1;
	degree[p.first]++;
}
```

尋找入度等於0的節點, 返回-1表示沒有找到:

```cpp
int findZero(const vector<int> &v) {
	int n = v.size();
	for (int i = 0; i < n; ++i) {
		if (v[i] == 0)
			return i;
	}
	return -1;
}
```

實現拓撲排序判斷:

```cpp
bool canTopsort(const vector<pair<int, int>> &request, vector<int> &degree) {
	int n = degree.size();
	vector<bool> visited(n, false);
	int sum = 0;
	while (sum < n) { // 還沒有訪問完
		int cur = findZero(degree);
		if (cur >= 0) { // 訪問節點cur
			sum++;
			degree[cur] = -1; // 標記當前節點為已經訪問狀態
			for (auto p : request) {
				if (p.second == cur)
					degree[p.first]--; // 去掉已訪問節點
			}
		} else
			return false;
	}
	return true;
}
```

以上方法實質就是BFS，也可以使用DFS，從一個節點出發，順著邊往下走，若回到已經訪問的節點，說明存在環。

為了標示正在訪問的節點、未訪問節點和已經訪問節點，我們分別使用-1, 0, 1表示。

```cpp
bool dfs(const vector<pair<int, int>> &request, vector<int> &visited, int i) {
	if (visited[i] == -1) // 回到了出發點，說明存在環
		return false;
	if (visited[i] == 1) // 已經訪問過該節點了
		return true;
	visited[i] = -1; // -1 表示正在訪問
	for (auto p : request) {
		if (p.second == i) {
			if (!dfs(request, visited, p.first))  // 訪問下一個節點
				return false;
		}
	}
	visited[i] = 1; // 標識為已經訪問過
	return true;
}

```
## 擴展

[Course Schedule II](../CourseScheduleII): 輸出拓撲排序結果
