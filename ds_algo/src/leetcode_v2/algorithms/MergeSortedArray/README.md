## Merge Sorted Array

Given two sorted integer arrays nums1 and nums2, merge nums2 into nums1 as one sorted array.

Note:
You may assume that nums1 has enough space (size that is greater or equal to m + n) to hold additional elements from nums2. The number of elements initialized in nums1 and nums2 are m and n respectively.

## Solution

使用兩個指針分別指向兩個數組，然後找到小的值作為結果的候選值。

為了避免移動數據，可以預先開一個新的數組，保存結果數組。

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

以上方法需要O(n)的空間來避免num1的移動。

如果從後面開始合併，這不需要O(n)的空間.

初始化`total = m + n - 1, i = m - 1, j = n - 1`, 向前掃描:

* 若`num1[i] >= num2[j]`, 則`num1[total--] = num1[i--]`
* 若`num2[j] > num1[i]`, 則`num1[total--] = num2[j--]`
* 重複以上步驟直到num1或者num2結束

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

以上方法，避免了O(n)的臨時空間。

## 擴展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合併兩個鏈表
* [Merge k Sorted Lists](../MergekSortedLists): 合併K個鏈表
* [Merge Sorted Array](../MergeSortedArray): 合併兩個數組
* [Sort List](../SortList): 歸併排序兩個鏈表
