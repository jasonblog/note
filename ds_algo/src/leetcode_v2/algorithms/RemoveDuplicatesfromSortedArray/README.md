## Remove Duplicates from Sorted Array
 Given a sorted array, remove the duplicates in place such that each element appear only once and return the new length.

Do not allocate extra space for another array, you must do this in place with constant memory.

For example,
```
Given input array A = [1,1,2],

Your function should return length = 2, and A is now [1,2]. 
```

## Solution
```c
int removeDuplicates(int a[], int n) {
	int i, j;
	if (n == 0)
		return 0;
	for (i = 1, j = 0; i < n; ++i) {
		if (a[i] != a[i - 1]) {
			a[++j] = a[i];
		}
	}
	return j + 1;
}
```
