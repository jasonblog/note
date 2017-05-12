## Rotate Array

Rotate an array of n elements to the right by k steps.

For example, with n = 7 and k = 3, the array `[1,2,3,4,5,6,7]` is rotated to `[5,6,7,1,2,3,4]`.

Note:
Try to come up as many solutions as you can, there are at least 3 different ways to solve this problem.

Hint:

Could you do it in-place with O(1) extra space?

Related problem: Reverse Words in a String II

## Solution

[列表旋转算法](https://github.com/krystism/algorithms/tree/master/rotate)

## Code
```c
void swap(int *a, int *b)
{
	if (a == NULL || b == NULL || a == b)
		return;
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}
void reverse(int a[], int s, int t)
{
	int i = s, j = t;
	while (i < j)
	{
		swap(&a[i++], &a[j--]);
	}
}
void rotate(int a[], int n, int k)
{
	k %= n;
	reverse(a, 0, n - 1);
	reverse(a, 0, k - 1);
	reverse(a, k, n - 1);
}
```
