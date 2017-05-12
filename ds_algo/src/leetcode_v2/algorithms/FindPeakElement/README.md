## Find Peak Element

A peak element is an element that is greater than its neighbors.

Given an input array where num[i] ≠ num[i+1], find a peak element and return its index.

The array may contain multiple peaks, in that case return the index to any one of the peaks is fine.

You may imagine that `num[-1] = num[n] = -∞`.

For example, in array `[1, 2, 3, 1]`, `3` is a peak element and your function should return the index number `2`.

click to show spoilers.
Note:

Your solution should be in logarithmic complexity.

Credits:
Special thanks to @ts for adding this problem and creating all test cases.


## Solution 1

题目的意思就是找一个局部最大值，即一个元素大于两边相邻的元素，该元素就是Peak Element。

直接搜索数组，时间复杂度O(n).
```cpp
int findPeakElement(vector<int> &a) {
	int n = a.size();
	if (n < 2 || a[0] > a[1])
		return 0;
	if (a[n - 1] > a[n - 2])
		return n - 1;
	for (int i = 1; i < n - 1; ++i)
		if (a[i] > a[i - 1] && a[i] > a[i + 1])
			return i;
}
```

## Solution 2

方案1比较次数是2n，实际上从0开始遍历，只需要找到一个元素`a[i] > a[i + 1]`即可，i就是局部最大值。

这是因为`a[-1]`是负无穷的，如果`a[0] > a[1]`，则a[0]就是Peak元素，否则必有`a[1] > a[0]`, 如果a[1] > a[2]，那
`a[1]`显然是Peak元素，否则`a[1] < a[2]`, 即`a[2] > a[1]`，判断a[2]和a[3]的关系，依次类推，`a[n - 1]`必然小于`a[n]`。
因此Peak元素必然存在
```cpp
int findPeakElement(vector<int> &a) {
	int n = a.size();
	for (int i = 0; i < n - 1; ++i)
		if (a[i] > a[i + 1])
			return i;
	return n - 1;
}
```

## Solution 3

方案2只是减少了比较次数，但复杂度依然是O(n), 而题目要求是O(logn)。

但凡想到O(xxlogn)算法，自然想到的是二分法，当二分法一般针对的是有序集合(有序数组、查找树等),而目前情况都不满足。

但是根据前面方案2的思路，如果`a[i] > a[i + 1]`，那a[i]可能就是局部最优，如果`a[i] < a[i + 1]`，那`a[i + 1]`是可能的局部最优.

根据这个原理，我们就可以自然想到二分法, 设`left = 0, right = n - 1, mid = (left + right) >> 1`, 则:

* `a[mid] > a[mid + 1] && a[mid] > a[mid - 1]`, 返回mid, 否则下一步
* `a[mid] > a[mid + 1], right = mid`, 否则下一步
* `a[mid] < a[mid - 1], left = mid + 1`。
* 重复直到`left == right`, 此时left就是Peak元素索引

```cpp
int fastFindPeakElement(vector<int> &a) {
	int n = a.size();
	int left = 0, right = n - 1;
	while (left < right) {
		int mid = (left + right) >> 1;
		printf("left = %d, mid = %d, right = %d\n", left, mid, right);
		if (a[mid] > a[mid - 1] && a[mid] > a[mid + 1])
			return mid;
		if (a[mid] > a[mid + 1])
			right = mid;
		else
			left = mid + 1;
	}
	return left;
}
```
