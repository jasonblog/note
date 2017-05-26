## Contains Duplicate

Given an array of integers, find if the array contains any duplicates. Your function should return true if any value appears at least twice in the array, and it should return false if every element is distinct.

## Solution

直接使用hashtable即可

```cpp
bool containsDuplicate(vector<int> &nums) {
	unordered_set<int> set;
	for (int i : nums) {
		if (set.find(i) != set.end())
			return true;
		else
			set.insert(i);
	}
	return false;
}
```

## 擴展

* [Contains Duplicate](../ContainsDuplicate): 判斷一個數組是否存在重複元素
* [Contains Duplicate II](../ContainsDuplicateII):判斷一個數組在指定距離內是否存在重複元素
* [Contains Duplicate III](../ContainsDuplicateIII):判斷一個數組在指定距離內是否存在元素距離小於t
