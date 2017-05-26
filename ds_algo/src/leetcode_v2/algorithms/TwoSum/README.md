# Two Sum

## Problem
Given an array of integers, find two numbers such that they add up to a specific target number.

The function twoSum should return indices of the two numbers such that they add up to the target, where index1 must be less than index2. Please note that your returned answers (both index1 and index2) are not zero-based.

You may assume that each input would have exactly one solution.

Input: numbers={2, 7, 11, 15}, target=9
Output: index1=1, index2=2

## Solution 1

暴力枚舉法，時間複雜度O(n<sup>2</sup>),時間複雜度O(1)

## Solution 2

假設數組為2 7 11 15， target 為9, 則用9減數組的元素得到一個新的數組7 2 -2 -6，則只需要查找數組中的元素是否在新的數組中出現即可判斷是否存在滿足條件的兩個數。

比如2和7 在新的數組中也存在，所以2和7即滿足條件。

題目要求找到滿足條件的索引，只需要用一個map，key為target-a[i]，value為index。當遍歷數組a[i]，若a[i]在map keyset中，則說明i和map[a[i]]即是解。

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
時間複雜度為O(n), 空間複雜度O(n).
