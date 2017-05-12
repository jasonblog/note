## Find Minimum in Rotated Sorted Array

Suppose a sorted array is rotated at some pivot unknown to you beforehand.

(i.e., `0 1 2 4 5 6 7` might become `4 5 6 7 0 1 2`).

Find the minimum element.

You may assume no duplicate exists in the array.

## Solution

使用二分搜索，设当前搜索范围为`[left, right]`, 则中间的值为`mid = (left + right) /2`, 则

* 若`a[left] < a[right]`, 说明没有旋转，返回`a[left]`.
* 若`a[mid] > a[right]`, 则结果在右边， `left = mid + 1`
* 若`a[mid] < a[right]`, 则结果在左边， `right = mid`

关键注意边界为题, 什么时候用`>=`什么时候用`>`, 以及什么时候用`mid`, 什么时候用`mid + 1`

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
		if (nums[mid] > nums[t])
			s = mid + 1;
		else 
			t = mid;
	}
	return nums[s];
}
```

## 扩展

1. 当有重复元素存在时，见[Find Minimum in Rotated Sorted Array II](../FindMinimuminRotatedSortedArray2)

2. [Search in Rotated Sorted Array](../SearchinRotatedSortedArray), 在旋转列表中查找某个数
