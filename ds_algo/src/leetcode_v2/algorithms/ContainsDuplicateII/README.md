## Contains Duplicate II

Given an array of integers and an integer k, find out whether there there are two distinct indices i and j in the array such that nums[i] = nums[j] and the difference between i and j is at most k.

## Solution

使用hashmap，key存储值，value存储索引, 若`nums[i]`在hashmap中，则若`i - map[nums[i]] <= k`, 返回true

```cpp
bool containsNearbyDuplicate(vector<int> &nums, int k) {
	unordered_map<int, int> map;
	int n = nums.size();
	for (int i = 0; i < n; ++i) {
		if (map.find(nums[i]) != map.end()) {
			int s = map[nums[i]];
			if (i - s <= k)
				return true;
		}
		map[nums[i]] = i;
	}
	return false;
}
```

## 扩展

* [Contains Duplicate](../ContainsDuplicate): 判断一个数组是否存在重复元素
* [Contains Duplicate II](../ContainsDuplicateII):判断一个数组在指定距离内是否存在重复元素
* [Contains Duplicate III](../ContainsDuplicateIII):判断一个数组在指定距离内是否存在元素距离小于t
