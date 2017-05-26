## Longest Consecutive Sequence

Given an unsorted array of integers, find the length of the longest consecutive elements sequence.

For example,
Given `[100, 4, 200, 1, 3, 2]`,
The longest consecutive elements sequence is `[1, 2, 3, 4]`. Return its length: `4`.

Your algorithm should run in O(n) complexity. 

## Solution

剛開始以為是DP，沒有思路！

看了答案發現其實並不是很複雜，關鍵找對思路。。

使用map，key為數值，value為長度, 設當前遍歷數組的值為i

* 若i已經在map中，忽略。
* 否則`left = map[i - 1] or 0, right = map[i + 1] or 0`,則`map[i] = left + right + 1`,即當前值的長度等於它左相鄰和右相鄰長度之和+1.
* 同時需要更新邊界值，即`map[i - left] = map[i], map[i + right] = map[i]`, 
因為我們第一步忽略了已經存在的值，因此我們只需要維護邊界值即可，而不用把中間的值同時更新.
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
