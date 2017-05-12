## 3Sum

Given an array S of n integers, are there elements a, b, c in S such that a + b + c = 0? Find all unique triplets in the array which gives the sum of zero.

Note:
Elements in a triplet (a,b,c) must be in non-descending order. (ie, a ≤ b ≤ c)
The solution set must not contain duplicate triplets.
```
    For example, given array `S = {-1 0 1 2 -1 -4}`,

    A solution set is:
    (-1, 0, 1)
    (-1, -1, 2)
```

## Solution 

两个数之和等于一个数的问题扩展。

先排序，依次遍历每个数`a[i]`，在后面的数组中找到两个数之和等于`-a[i]`。

## Code
```c
class Solution {
	public:
		vector<vector<int>> threeSum(vector<int> &num) {
			sort(num.begin(), num.end());
			vector<vector<int>> result;
			auto n = num.size();
			size_t i;
			for (i = 0; i < n; ++i) {
				if (i > 0 && num[i] == num[i - 1])
					continue;
				int a = num[i];
				int low = i + 1, high = n - 1;
				while (low < high) {
					int b = num[low];
					int c = num[high];
					if (a + b + c == 0) {
						vector<int> v = {a, b, c};
						result.push_back(v);
						while (low + 1 < n && num[low] == num[low + 1]) low++;
						while (high - 1 >= 0 && num[high] == num[high - 1]) high--;
						low++;
						high--;
					} else if (a + b + c > 0) {
						while (high - 1 >= 0 && num[high] == num[high - 1]) high--;
						high--;
					} else {
						while (low + 1 < n && num[low] == num[low + 1]) low++;
						low++;
					}
				}
			}
			return result;
		}
};
```
