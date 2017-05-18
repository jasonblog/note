## Longest Consecutive Sequence

Given an unsorted array of integers, find the length of the longest consecutive elements sequence.

For example,
Given `[100, 4, 200, 1, 3, 2]`,
The longest consecutive elements sequence is `[1, 2, 3, 4]`. Return its length: `4`.

Your algorithm should run in O(n) complexity. 

## Solution

刚开始以为是DP，没有思路！

看了答案发现其实并不是很复杂，关键找对思路。。

使用map，key为数值，value为长度, 设当前遍历数组的值为i

* 若i已经在map中，忽略。
* 否则`left = map[i - 1] or 0, right = map[i + 1] or 0`,则`map[i] = left + right + 1`,即当前值的长度等于它左相邻和右相邻长度之和+1.
* 同时需要更新边界值，即`map[i - left] = map[i], map[i + right] = map[i]`, 
因为我们第一步忽略了已经存在的值，因此我们只需要维护边界值即可，而不用把中间的值同时更新.
* 返回最大的map值即可


```cpp
int longestConsecutive(vector<int> &nums) {
	unordered_map<int, int> map;
	int res = 0;
	for (int i : nums) {
		if (map.find(i) != map.end()) {
			continue;
		}
		int left = getOrElse(map, i - 1);
		int right = getOrElse(map, i + 1);
		int sum = left + right + 1;
		res = max(res, sum);
		map[i] = sum;
		map[i - left] = sum;
		map[i + right] = sum;
	}
	return res;
}
```
