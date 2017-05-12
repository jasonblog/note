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

此时使用递归不好解。使用迭代的方法，见[Subsets](../Subsets), 此时需要改一下。

以`[1,2,2,2]`为例，前面的1,2均无重复，因此按照正常过程走，得到result为

```
[]
[1]
[2]
[1,2]
```
此时处理第2个2, 我们需要排重，显然`[] += 2 == [2]`,与第三个元素`[2]`重复，应该忽略。
同理`[1] += 2 == [1,2]`, 也应该重复。后面的[2] += 2 == [2,2], [1,2] += 2 == [1,2,2]均无重复, 即

```
[]
[1]
**********以下为不重复部分*******
[2]
[1,2]
```

因此结果为:

```
[]
[1]
[2]
[1,2]
[2,2]
[1,2,2]
```

我们处理第3个2,很容易得到下面的结果:

```
[]
[1]
[2]
[1,2]
**********以下为不重复部分*******
[2,2]
[1,2,2]
```

我们发现当有重复元素时，只有上一次新加的元素需要更新，其他的不需要更新，否则重复, 因此需要找出上一次更新的元素。

在[Subsets](../Subsets)中，算法为:

```cpp
for (int i = 0; i < n; ++i) {
	int m = result.size();
	for (int j = 0; j < m; ++j) {
		result.push_back(result[j]);
		result.back().push_back(nums[i]);
	}
}

```

第二个循环j是从0开始遍历result数组，当有重复元素时，我们只需要调整j的初始值，使其从上次新加的元素开始即可，
显然它等于上次还没有添加元素时的长度.

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
