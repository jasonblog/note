## Median of Two Sorted Arrays

There are two sorted arrays nums1 and nums2 of size m and n respectively. Find the median of the two sorted arrays. The overall run time complexity should be O(log (m+n)).

## Solution

這個題目可以轉化成求兩個有序數組的第K大的數.

定義切片a[i:j]表示a[i],a[i+1],...,a[j-1]的所有元素。

假設a,b兩個數組的元素都大於`k/2`,我們將a和b的第`k/2`個元素比較，有三種情況:

1. `a[k/2 - 1] == b[k/2 - 1]`, a，b前面都有 k/2 - 1個元素(共k-2個數)比這兩個數小，則兩個數都是topk的數，直接返回a[k/2 - 1]或者b[k/2 - 1].
2. `a[k/2 - 1] > b[k/2 - 1]`, b[0:k/2]一定在a和b並集的topk以內，不可能包括第k大的數，因此可以丟掉b[0:k/2]，然後求剩下元素的第`k - k/2 == k/2`的數.
3. `a[k/2 - 1] < b[k/2 - 1]`, a[0:k/2]一定在a和b並集的topk以內，因此不可能包括第k大的數，因此可以丟掉a[0:k/2],然後求剩下元素的第k/2的數.

若a[k/2 - 1] < b[k/2 - 1]，意味著a[0:k/2]肯定在a和b的topk元素範圍以內，即第k大的數不可能在這個範圍以內,我們可以把a[0:k/2]這些元素丟掉.


以上是基於a，b元素個數都大於k/2，如果數組元素個數小於k/2,則整個數組一起比較，即直接比較最後一個元素。不妨設a元素個數為n，且n < k / 2，則

1. a[n - 1] == b[k/2 - 1], 丟掉a的所有元素，求b中第k - n的元素
2. a[n - 1] > b[k/2 - 1],丟掉b[0:k/2].
3. a[n - 1] < b[k/2 - 1],丟掉a的所有元素。


把二者統一起來,不妨設兩個數組的長度分別為n, m, 且n <= m，則

設`i = min(k/2, n)`, `j = min(k/2, m)`:

1. a[i - 1] > b[j - 1], 丟掉b[0:j]
2. a[i - 1] <= b[j - 1], 丟掉a[0:i]


於是求第k大的數為:
```c
int getkth(int a[], int n, int b[], int m, int k)
{
	if (k <= 0 || k > n + m)
		return -1;
	if (n > m)
		return getkth(b, m, a, n, k);
	if (n == 0)
		return b[k - 1];
	if (k == 1) {
		return min(a[0], b[0]);
	}
	int i = min(n, k / 2), j = min(m, k / 2);
	if (a[i - 1] > b[j - 1])
		return getkth(a, n, b + j, m - j, k - j);
	else
		return getkth(a + i, n - i, b, m, k - i);
}
```

求出了第k個數，要求中位數，就容易了。

1. 若元素總數n為奇數，則只有一箇中位數，即求第n/2 + 1大的數
2. 若元素總數n為偶數，則中間有兩個數，需要求平均值, 即`median = (getkth(n/2) + getkth(n/2+1))/2`
```c
double findMedianSortedArrays(int *a, int n, int *b, int m) {
	if ((n + m) & 0x1) {
		return getkth(a, n, b, m, ((n + m) >> 1) + 1);
	}
	int left = getkth(a, n, b, m, (n + m) >> 1);
	int right = getkth(a, n, b, m, ((n + m) >> 1) + 1);
	return (left + right) / 2.0;
}
```
