## Contains Duplicate II

Given an array of integers and an integer k, find out whether there there are two distinct indices i and j in the array such that nums[i] = nums[j] and the difference between i and j is at most k.

## Solution

使用hashmap，key存儲值，value存儲索引, 若`nums[i]`在hashmap中，則若`i - map[nums[i]] <= k`, 返回true

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

## 擴展

* [Contains Duplicate](../ContainsDuplicate): 判斷一個數組是否存在重複元素
* [Contains Duplicate II](../ContainsDuplicateII):判斷一個數組在指定距離內是否存在重複元素
* [Contains Duplicate III](../ContainsDuplicateIII):判斷一個數組在指定距離內是否存在元素距離小於t
