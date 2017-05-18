## Subsets

Given a set of distinct integers, nums, return all possible subsets.

Note:

* Elements in a subset must be in non-descending order.
* The solution set must not contain duplicate subsets.

For example,
If nums = `[1,2,3]`, a solution is:

```
[
  [3],
  [1],
  [2],
  [1,2,3],
  [1,3],
  [2,3],
  [1,2],
  []
]
```

## Solution

使用回溯递归:

```cpp
vector<vector<int>> subsets_recurisive(vector<int> &nums) {
	vector<vector<int>> result;
	vector<int> cur;
	sort(nums.begin(), nums.end());
	subsets_recurisive(result, cur, nums, 0);
	return result;
}
void subsets_recurisive(vector<vector<int>> &result, vector<int> cur, const vector<int> &nums, int i) {
	if (i == nums.size()) {
		result.push_back(cur);
		return;
	}
	subsets_recurisive(result, cur, nums, i + 1); // i is not included
	cur.push_back(nums[i]); 
	subsets_recurisive(result, cur, nums, i + 1); // i in included
}
```

递归方法更直接，容易理解，缺点是需要空间保存调用栈。

可以使用迭代的方法:

假设我们已经处理完前面的元素，保存在result中，初始化为只有一个元素`[]`,当前处理第i个元素:

遍历所有的result元素Ri， 拷贝Ri副本为Rj， `Rj.push_back(i)`, 再把Rj压入result中，相当于result每个元素一分为二，
一个包含当前值，一个不包含当前值.

```cpp
vector<vector<int>> subsets_iterative(vector<int> &nums) {
	vector<vector<int>> result(1, vector<int>());
	sort(begin(nums), end(nums));
	int n = nums.size();
	for (int i = 0; i < n; ++i) {
		int m = result.size();
		for (int j = 0; j < m; ++j) {
			result.push_back(result[j]); // copy, not include current number
			result.back().push_back(nums[i]); // include current number
		}
	}
	return result;
}
```

## 扩展

[Subsets II](../SubsetsII), 有重复元素的情况
