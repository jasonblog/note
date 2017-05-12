## Kth Largest Element in an Array

Find the kth largest element in an unsorted array. Note that it is the kth largest element in the sorted order, not the kth distinct element.

For example,
Given `[3,2,1,5,6,4]` and k = 2, return 5.

Note: 
You may assume k is always valid, 1 ≤ k ≤ array's length.

Credits:
Special thanks to @mithmatt for adding this problem and creating all test cases.

## Solution

利用快排法的partition函数:

```cpp
int partition(vector<int> &nums, int s, int t)
{
	int i = s, j = t;
	int k = nums[s];
	while (i < j) {
		while (nums[j] <= k && i < j) --j;
		if (i < j) {
			nums[i] = nums[j];
		}
		while (nums[i] >= k && i < j) ++i;
		if (i < j) {
			nums[j] = nums[i];
		}
	}
	nums[i] = k;
	return i;
}
```

设返回的位置为pos，这个pos的值一定是排序后的正确顺序，因此若:

* pos == k - 1, 则直接返回pos位置的值即可
* 若pos < k - 1, 则在左边查找
* 否则在右边查找.

```cpp
int findKthLargest(vector<int> &nums, int k) {
	int s = 0, t = nums.size() - 1;
	while (true) {
		int pos = partition(nums, s, t);
		if (pos == k - 1)
			return nums[pos];
		if (pos > k - 1) {
			t = pos - 1;
		} else {
			s = pos + 1;
		}
	}
	return -1;
}
```
