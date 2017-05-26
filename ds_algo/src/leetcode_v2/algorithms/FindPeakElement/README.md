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

題目的意思就是找一個局部最大值，即一個元素大於兩邊相鄰的元素，該元素就是Peak Element。

直接搜索數組，時間複雜度O(n).
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

方案1比較次數是2n，實際上從0開始遍歷，只需要找到一個元素`a[i] > a[i + 1]`即可，i就是局部最大值。

這是因為`a[-1]`是負無窮的，如果`a[0] > a[1]`，則a[0]就是Peak元素，否則必有`a[1] > a[0]`, 如果a[1] > a[2]，那
`a[1]`顯然是Peak元素，否則`a[1] < a[2]`, 即`a[2] > a[1]`，判斷a[2]和a[3]的關係，依次類推，`a[n - 1]`必然小於`a[n]`。
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

方案2只是減少了比較次數，但複雜度依然是O(n), 而題目要求是O(logn)。

但凡想到O(xxlogn)算法，自然想到的是二分法，當二分法一般針對的是有序集合(有序數組、查找樹等),而目前情況都不滿足。

但是根據前面方案2的思路，如果`a[i] > a[i + 1]`，那a[i]可能就是局部最優，如果`a[i] < a[i + 1]`，那`a[i + 1]`是可能的局部最優.

根據這個原理，我們就可以自然想到二分法, 設`left = 0, right = n - 1, mid = (left + right) >> 1`, 則:

* `a[mid] > a[mid + 1] && a[mid] > a[mid - 1]`, 返回mid, 否則下一步
* `a[mid] > a[mid + 1], right = mid`, 否則下一步
* `a[mid] < a[mid - 1], left = mid + 1`。
* 重複直到`left == right`, 此時left就是Peak元素索引

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
