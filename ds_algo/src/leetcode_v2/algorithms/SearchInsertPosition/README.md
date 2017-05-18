## Search Insert Position

Given a sorted array and a target value, return the index if the target is found. If not, return the index where it would be if it were inserted in order.

You may assume no duplicates in the array.

Here are few examples.
```
[1,3,5,6], 5 → 2
[1,3,5,6], 2 → 1
[1,3,5,6], 7 → 4
[1,3,5,6], 0 → 0
```

## Solution

二分搜索，若没有找到，则最后s == t, s即是需要插入的位置

## Code
```c
int searchIndex(int A[], int n, int target)
{
	if (A == NULL || n < 1)
		return 0;
	int s = 0, t = n - 1;
	while (s <= t) {
		int mid = (s + t) >> 1;
		if (A[mid] == target)
			return mid;
		if (A[mid] > target) {
			t = mid - 1;
		} else {
			s = mid + 1;
		}
	}
	return s;
}
```
