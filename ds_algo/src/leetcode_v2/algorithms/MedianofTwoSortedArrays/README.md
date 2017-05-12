## Median of Two Sorted Arrays

There are two sorted arrays nums1 and nums2 of size m and n respectively. Find the median of the two sorted arrays. The overall run time complexity should be O(log (m+n)).

## Solution

这个题目可以转化成求两个有序数组的第K大的数.

定义切片a[i:j]表示a[i],a[i+1],...,a[j-1]的所有元素。

假设a,b两个数组的元素都大于`k/2`,我们将a和b的第`k/2`个元素比较，有三种情况:

1. `a[k/2 - 1] == b[k/2 - 1]`, a，b前面都有 k/2 - 1个元素(共k-2个数)比这两个数小，则两个数都是topk的数，直接返回a[k/2 - 1]或者b[k/2 - 1].
2. `a[k/2 - 1] > b[k/2 - 1]`, b[0:k/2]一定在a和b并集的topk以内，不可能包括第k大的数，因此可以丢掉b[0:k/2]，然后求剩下元素的第`k - k/2 == k/2`的数.
3. `a[k/2 - 1] < b[k/2 - 1]`, a[0:k/2]一定在a和b并集的topk以内，因此不可能包括第k大的数，因此可以丢掉a[0:k/2],然后求剩下元素的第k/2的数.

若a[k/2 - 1] < b[k/2 - 1]，意味着a[0:k/2]肯定在a和b的topk元素范围以内，即第k大的数不可能在这个范围以内,我们可以把a[0:k/2]这些元素丢掉.


以上是基于a，b元素个数都大于k/2，如果数组元素个数小于k/2,则整个数组一起比较，即直接比较最后一个元素。不妨设a元素个数为n，且n < k / 2，则

1. a[n - 1] == b[k/2 - 1], 丢掉a的所有元素，求b中第k - n的元素
2. a[n - 1] > b[k/2 - 1],丢掉b[0:k/2].
3. a[n - 1] < b[k/2 - 1],丢掉a的所有元素。


把二者统一起来,不妨设两个数组的长度分别为n, m, 且n <= m，则

设`i = min(k/2, n)`, `j = min(k/2, m)`:

1. a[i - 1] > b[j - 1], 丢掉b[0:j]
2. a[i - 1] <= b[j - 1], 丢掉a[0:i]


于是求第k大的数为:
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

求出了第k个数，要求中位数，就容易了。

1. 若元素总数n为奇数，则只有一个中位数，即求第n/2 + 1大的数
2. 若元素总数n为偶数，则中间有两个数，需要求平均值, 即`median = (getkth(n/2) + getkth(n/2+1))/2`
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
