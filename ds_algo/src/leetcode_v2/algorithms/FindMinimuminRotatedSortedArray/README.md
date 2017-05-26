## Find Minimum in Rotated Sorted Array

Suppose a sorted array is rotated at some pivot unknown to you beforehand.

(i.e., `0 1 2 4 5 6 7` might become `4 5 6 7 0 1 2`).

Find the minimum element.

You may assume no duplicate exists in the array.

## Solution

使用二分搜索，設當前搜索範圍為`[left, right]`, 則中間的值為`mid = (left + right) /2`, 則

* 若`a[left] < a[right]`, 說明沒有旋轉，返回`a[left]`.
* 若`a[mid] > a[right]`, 則結果在右邊， `left = mid + 1`
* 若`a[mid] < a[right]`, 則結果在左邊， `right = mid`

關鍵注意邊界為題, 什麼時候用`>=`什麼時候用`>`, 以及什麼時候用`mid`, 什麼時候用`mid + 1`

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

## 擴展

1. 當有重複元素存在時，見[Find Minimum in Rotated Sorted Array II](../FindMinimuminRotatedSortedArray2)

2. [Search in Rotated Sorted Array](../SearchinRotatedSortedArray), 在旋轉列表中查找某個數
