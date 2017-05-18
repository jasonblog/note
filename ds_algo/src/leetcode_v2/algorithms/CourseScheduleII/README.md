## Course Schedule II

There are a total of n courses you have to take, labeled from 0 to n - 1.

Some courses may have prerequisites, for example to take course 0 you have to first take course 1, which is expressed as a pair: [0,1]

Given the total number of courses and a list of prerequisite pairs, return the ordering of courses you should take to finish all courses.

There may be multiple correct orders, you just need to return one of them. If it is impossible to finish all courses, return an empty array.

For example:

```
2, [[1,0]]
```

There are a total of 2 courses to take. To take course 1 you should have finished course 0. So the correct course order is `[0,1]`

```
4, [[1,0],[2,0],[3,1],[3,2]]
```

There are a total of 4 courses to take. To take course 3 you should have finished both courses 1 and 2. Both courses 1 and 2 should be taken after you finished course 0. So one correct course order is `[0,1,2,3]`. Another correct ordering is`[0,2,1,3]`.

Note:
*The input prerequisites is a graph represented by a list of edges, not adjacency matrices.*

Hints:

* This problem is equivalent to finding the topological order in a directed graph. If a cycle exists, no topological ordering exists and therefore it will be impossible to take all courses.
* Topological Sort via DFS - A great video tutorial (21 minutes) on Coursera explaining the basic concepts of Topological Sort.
* Topological sort could also be done via BFS.

## Solution

求拓扑排序结果, 算法见[Course Schedule](../CourseSchedule)

```cpp
vector<int> topsort(int n, const vector<pair<int, int>> &request) {
	vector<int> result;
	vector<int> degree(n, 0);
	for (auto p : request) {
		degree[p.first]++;
	}
	while (result.size() < n) { // 还没有访问完
		int cur = findZero(degree);
		if (cur >= 0) { // 访问节点cur
			result.push_back(cur);
			degree[cur] = -1; // 标记当前节点为已经访问状态
			for (auto p : request) {
				if (p.second == cur)
					degree[p.first]--; // 去掉已访问节点
			}
		} else
			return vector<int>();
	}
	return result;
}
```
