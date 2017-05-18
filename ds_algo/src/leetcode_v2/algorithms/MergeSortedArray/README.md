## Merge Sorted Array

Given two sorted integer arrays nums1 and nums2, merge nums2 into nums1 as one sorted array.

Note:
You may assume that nums1 has enough space (size that is greater or equal to m + n) to hold additional elements from nums2. The number of elements initialized in nums1 and nums2 are m and n respectively.

## Solution

使用两个指针分别指向两个数组，然后找到小的值作为结果的候选值。

为了避免移动数据，可以预先开一个新的数组，保存结果数组。

```cpp
void merge_from_start(vector<int> &num1, int m, vector<int> &num2, int n) {
	int i = 0,j = 0, k = 0;
	vector<int> result(m + n, 0);
	while (i < m && j < n) {
		if (num1[i] <= num2[j]) {
			result[k++] = num1[i++];
		} else {
			result[k++] = num2[j++];
		}
	}
	while (i < m) {
		result[k++] = num1[i++];
	}
	while (j < n) {
		result[k++] = num2[j++];
	}
	num1.clear();
	for_each(begin(result), end(result), [&num1](int i){num1.push_back(i);});
		}
```

以上方法需要O(n)的空间来避免num1的移动。

如果从后面开始合并，这不需要O(n)的空间.

初始化`total = m + n - 1, i = m - 1, j = n - 1`, 向前扫描:

* 若`num1[i] >= num2[j]`, 则`num1[total--] = num1[i--]`
* 若`num2[j] > num1[i]`, 则`num1[total--] = num2[j--]`
* 重复以上步骤直到num1或者num2结束

```cpp
void merge_from_end(vector<int> &num1, int m, vector<int> &num2, int n) {
	int k = m + n - 1;
	int i = m - 1, j = n - 1;
	while (i >= 0 && j >= 0) {
		if (num1[i] >= num2[j])
			num1[k--] = num1[i--];
		else
			num1[k--] = num2[j--];
	}
	while (j >= 0)
		num1[k--] = num2[j--];
}
```

以上方法，避免了O(n)的临时空间。

## 扩展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合并两个链表
* [Merge k Sorted Lists](../MergekSortedLists): 合并K个链表
* [Merge Sorted Array](../MergeSortedArray): 合并两个数组
* [Sort List](../SortList): 归并排序两个链表
