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

使用回溯遞歸:

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

遞歸方法更直接，容易理解，缺點是需要空間保存調用棧。

可以使用迭代的方法:

假設我們已經處理完前面的元素，保存在result中，初始化為只有一個元素`[]`,當前處理第i個元素:

遍歷所有的result元素Ri， 拷貝Ri副本為Rj， `Rj.push_back(i)`, 再把Rj壓入result中，相當於result每個元素一分為二，
一個包含當前值，一個不包含當前值.

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

## 擴展

[Subsets II](../SubsetsII), 有重複元素的情況
