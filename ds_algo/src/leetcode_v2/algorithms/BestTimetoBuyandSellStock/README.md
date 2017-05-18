## Best Time to Buy and Sell Stock

Say you have an array for which the ith element is the price of a given stock on day i.

If you were only permitted to complete at most one transaction (ie, buy one and sell one share of the stock), design an algorithm to find the maximum profit.

## Solution

本质就是求数对之差最大值，只需要保存前面的最小值，然后当前值减去最小值一定是最大的

```c
int maxProfit(int *a, int n)
{
	int min = a[0];
	int max = 0;
	for (int i = 1; i < n; ++i) {
		min = MIN(min, a[i]);
		max = MAX(max, a[i] - min);
	}
	return max;
}
```
