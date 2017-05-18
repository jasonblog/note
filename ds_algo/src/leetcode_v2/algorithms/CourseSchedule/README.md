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

显然这是一道拓扑排序题。求拓扑排序的算法过程:

* 求出所有节点的入度`in-degree`
* 搜索`in-degree`，找出入度为0的节点，若没有找到入读为0的节点，则一定存在环，返回false，否则，从节点中删除该节点，并与之相关联的所有有向边。
* 重复以上，直到所有的节点访问完毕，返回true

首先求出所有的入度(注意题目的图是后面指向前面):

```cpp
vector<int> degree(n, 0);
for (auto p : request) {
	//g[p.second][p.first] = 1;
	degree[p.first]++;
}
```

寻找入度等于0的节点, 返回-1表示没有找到:

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

实现拓扑排序判断:

```cpp
bool canTopsort(const vector<pair<int, int>> &request, vector<int> &degree) {
	int n = degree.size();
	vector<bool> visited(n, false);
	int sum = 0;
	while (sum < n) { // 还没有访问完
		int cur = findZero(degree);
		if (cur >= 0) { // 访问节点cur
			sum++;
			degree[cur] = -1; // 标记当前节点为已经访问状态
			for (auto p : request) {
				if (p.second == cur)
					degree[p.first]--; // 去掉已访问节点
			}
		} else
			return false;
	}
	return true;
}
```

以上方法实质就是BFS，也可以使用DFS，从一个节点出发，顺着边往下走，若回到已经访问的节点，说明存在环。

为了标示正在访问的节点、未访问节点和已经访问节点，我们分别使用-1, 0, 1表示。

```cpp
bool dfs(const vector<pair<int, int>> &request, vector<int> &visited, int i) {
	if (visited[i] == -1) // 回到了出发点，说明存在环
		return false;
	if (visited[i] == 1) // 已经访问过该节点了
		return true;
	visited[i] = -1; // -1 表示正在访问
	for (auto p : request) {
		if (p.second == i) {
			if (!dfs(request, visited, p.first))  // 访问下一个节点
				return false;
		}
	}
	visited[i] = 1; // 标识为已经访问过
	return true;
}

```
## 扩展

[Course Schedule II](../CourseScheduleII): 输出拓扑排序结果
