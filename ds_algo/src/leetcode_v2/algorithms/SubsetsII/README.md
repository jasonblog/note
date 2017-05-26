## Subsets II

Given a collection of integers that might contain duplicates, nums, return all possible subsets.

Note:

* Elements in a subset must be in non-descending order.
* The solution set must not contain duplicate subsets.

For example,
If nums = `[1,2,2]`, a solution is:

```
[
  [2],
  [1],
  [1,2,2],
  [2,2],
  [1,2],
  []
]
```

## Solution

此時使用遞歸不好解。使用迭代的方法，見[Subsets](../Subsets), 此時需要改一下。

以`[1,2,2,2]`為例，前面的1,2均無重複，因此按照正常過程走，得到result為

```
[]
[1]
[2]
[1,2]
```
此時處理第2個2, 我們需要排重，顯然`[] += 2 == [2]`,與第三個元素`[2]`重複，應該忽略。
同理`[1] += 2 == [1,2]`, 也應該重複。後面的[2] += 2 == [2,2], [1,2] += 2 == [1,2,2]均無重複, 即

```
[]
[1]
**********以下為不重複部分*******
[2]
[1,2]
```

因此結果為:

```
[]
[1]
[2]
[1,2]
[2,2]
[1,2,2]
```

我們處理第3個2,很容易得到下面的結果:

```
[]
[1]
[2]
[1,2]
**********以下為不重複部分*******
[2,2]
[1,2,2]
```

我們發現當有重複元素時，只有上一次新加的元素需要更新，其他的不需要更新，否則重複, 因此需要找出上一次更新的元素。

在[Subsets](../Subsets)中，算法為:

```cpp
for (int i = 0; i < n; ++i) {
	int m = result.size();
	for (int j = 0; j < m; ++j) {
		result.push_back(result[j]);
		result.back().push_back(nums[i]);
	}
}

```

第二個循環j是從0開始遍歷result數組，當有重複元素時，我們只需要調整j的初始值，使其從上次新加的元素開始即可，
顯然它等於上次還沒有添加元素時的長度.

即 `j = i > 0 && nums[i] == nums[i - 1] ? lastSize : 0;`


```cpp
vector<vector<int>> subsets_iterative(vector<int> &nums) {
	vector<vector<int>> result(1, vector<int>());
	sort(begin(nums), end(nums));
	int n = nums.size();
	int m = 0;
	for (int i = 0; i < n; ++i) {
		int j = i > 0 && nums[i] == nums[i - 1] ? m : 0;
		m = result.size();
		for (; j < m; ++j) {
			result.push_back(result[j]);
			result.back().push_back(nums[i]);
		}
	}
	return result;
}
```
