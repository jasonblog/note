## Maximum Subarray

Find the contiguous subarray within an array (containing at least one number) which has the largest sum.

For example, given the array [−2,1,−3,4,−1,2,1,−5,4],
the contiguous subarray [4,−1,2,1] has the largest sum = 6.

click to show more practice.
More practice:

If you have figured out the O(n) solution, try coding another solution using the divide and conquer approach, which is more subtle.

## Solution

最大连续之和经典题

```c
int maxSubArray(int *a, int n)
{
	if (a == NULL || n <= 0)
		return 0;
	int max = a[0], sum = a[0];
	for (int i = 1; i < n; ++i) {
		if (sum < 0)
			sum = a[i];
		else
			sum += a[i];
		max = MAX(max, sum);
	}
	return max;
}
```
## 扩展

[Maximum Product Subarray](../MaximumProductSubarray): 最大连续之积
