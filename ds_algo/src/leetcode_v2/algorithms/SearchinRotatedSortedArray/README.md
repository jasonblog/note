## Search in Rotated Sorted Array

Suppose a sorted array is rotated at some pivot unknown to you beforehand.

(i.e., `0 1 2 4 5 6 7` might become `4 5 6 7 0 1 2`).

You are given a target value to search. If found in the array return its index, otherwise return -1.

You may assume no duplicate exists in the array.

## Solution

既然是有序，自然想到二分搜索, 和[Find Minimum in Rotated Sorted Array](../FindMinimuminRotatedSortedArray)类似:

初始化`s = 0, t = n - 1`:

* 若`a[s] < a[t]`, 比如`1 2 3 4 5`,则整个列表有序，直接二分搜索即可.
* `mid = s + ((t - s) >> 1)`, 若`a[mid] == key`, 返回mid，结束
* `a[mid] < a[t]`, 比如`5 1 [2] 3 4`或`4 5 [1] 2 3`, 则右边一定是有序的:

	+ 若`a[mid] < key <= a[t]`, 二分搜索右边部分即可
	+ 否则搜索左边， `t = mid - 1`

* `a[mid] > a[t]`, 比如`3 4 [5] 1 2` 或者 `2 3 [4] 5 1`, 则左边一定是有序的:

	+ 若`a[s] <= key < a[mid]`, 二分搜索左边部分
	+ 否则搜索右边，`s = mid + 1`

首先实现二分搜索:

```c
int binarySearch(int *a, int s, int t, int key)
{
	while (s <= t) {
		int mid = s + ((t - s) >> 1);
		if (a[mid] == key)
			return mid;
		if (a[mid] > key)
			t = mid - 1;
		else
			s = mid + 1;
	}
	return -1;
}
```

然后使用以上算法实现:

```c
int search(int *a, int n, int key)
{
	int s = 0, t = n - 1;
	while (s <= t) {
		if (a[s] < a[t])
			return binarySearch(a, s, t, key);
		int mid = s + ((t - s) >> 1);
		//printf("s = %d, t = %d, mid = %d\n", s, t, mid);
		if (a[mid] == key)
			return mid;
		if (a[mid] < a[t]) {
			if (a[mid] < key && key <= a[t])
				return binarySearch(a, mid + 1, t, key);
			else
				t = mid - 1;
		} else { // a[mid] > a[t]
			if (a[s] <= key && key < a[mid]) // 若key 在s和mid之间
				return binarySearch(a, s, mid - 1, key);
			else
				s = mid + 1;
		}
	}
	return -1;
}
```

## 扩展

1. [Find Minimum in Rotated Sorted Array](../FindMinimuminRotatedSortedArray)

2. [Find Minimum in Rotated Sorted Array II](../FindMinimuminRotatedSortedArray2)

