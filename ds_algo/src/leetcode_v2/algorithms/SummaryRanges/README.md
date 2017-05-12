## Summary Ranges

Given a sorted integer array without duplicates, return the summary of its ranges.

For example, given [0,1,2,4,5,7], return ["0->2","4->5","7"].

Credits:
Special thanks to @jianchao.li.fighter for adding this problem and creating all test cases.

## Solution

直接遍历数组即可

```cpp
vector<string> summaryRanges(vector<int> &nums) {
	vector<string> result;
	int n = nums.size();
	if (n == 0) {
		return result;
	}
	int s = nums[0], t;
	bool update = false;
	int i;
	for (i = 1; i < n; ++i) {
		if (nums[i] == nums[i - 1] + 1) {
			update = true;
			t = nums[i];
		} else {
			if (update) {
				result.push_back(to_string(s) + "->" + to_string(t));
				s = nums[i];
				update = false;
			} else {
				result.push_back(to_string(s));
				s = nums[i];
			}
		}
	}
	// last update
	if (update) {
		result.push_back(to_string(s) + "->" + to_string(nums[n - 1]));
	} else {
		result.push_back(to_string(nums[n - 1]));
	}
	return result;
}
```
