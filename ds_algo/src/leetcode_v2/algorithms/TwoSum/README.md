# Two Sum

## Problem
Given an array of integers, find two numbers such that they add up to a specific target number.

The function twoSum should return indices of the two numbers such that they add up to the target, where index1 must be less than index2. Please note that your returned answers (both index1 and index2) are not zero-based.

You may assume that each input would have exactly one solution.

Input: numbers={2, 7, 11, 15}, target=9
Output: index1=1, index2=2

## Solution 1

暴力枚举法，时间复杂度O(n<sup>2</sup>),时间复杂度O(1)

## Solution 2

假设数组为2 7 11 15， target 为9, 则用9减数组的元素得到一个新的数组7 2 -2 -6，则只需要查找数组中的元素是否在新的数组中出现即可判断是否存在满足条件的两个数。

比如2和7 在新的数组中也存在，所以2和7即满足条件。

题目要求找到满足条件的索引，只需要用一个map，key为target-a[i]，value为index。当遍历数组a[i]，若a[i]在map keyset中，则说明i和map[a[i]]即是解。

```c
vector<int> twoSum(vector<int> &numbers, int target) {
	map<int, int> m;
	vector<int> result;
	for (size_t i = 0; i < numbers.size(); ++i) {
		if (m.find(numbers[i]) == m.end()) {
			m[target - numbers[i]] = i;
		} else {
			result.push_back(m[numbers[i]] + 1); // index from 1, not 0
			result.push_back(i + 1); // index from 1
		}
	}
	return result;
}
```
时间复杂度为O(n), 空间复杂度O(n).
