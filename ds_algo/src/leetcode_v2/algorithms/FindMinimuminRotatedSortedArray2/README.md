## Find Minimum in Rotated Sorted Array II

> Follow up for "Find Minimum in Rotated Sorted Array":
> What if duplicates are allowed?

> Would this affect the run-time complexity? How and why?

Suppose a sorted array is rotated at some pivot unknown to you beforehand.

(i.e., `0 1 2 4 5 6 7` might become `4 5 6 7 0 1 2`).

Find the minimum element.

The array may contain duplicates.

## Solution

和[Find Minimum in Rotated Sorted Array I](../FindMinimuminRotatedSortedArray) 類似，不過需要考慮`a[mid] == a[right]`的情況,

此時只需要把`right--`即可

## Code
```cpp
int findMin(vector<int> &nums) {
	int n = nums.size();
	if (n == 0)
		return INT_MIN;
	int s = 0, t = n - 1;
	while (s < t) {
		if (nums[s] < nums[t])
			return nums[s];
		int mid = (s + t) >> 1;
		if (nums[mid] > nums[t]) {
			s = mid + 1;
		} else if (nums[mid] < nums[t]) {
			t = mid;
		} else {
			t--;
		}
	}
	return nums[s];
}
```

## 擴展

[Search in Rotated Sorted Array](../SearchinRotatedSortedArray), 從旋轉列表中查找某個值

[Find Minimum in Rotated Sorted Array](../FindMinimuminRotatedSortedArray)
