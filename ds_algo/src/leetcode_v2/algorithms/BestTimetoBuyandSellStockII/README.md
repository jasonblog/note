## Best Time to Buy and Sell Stock II

Say you have an array for which the ith element is the price of a given stock on day i.

Design an algorithm to find the maximum profit. You may complete as many transactions as you like (ie, buy one and sell one share of the stock multiple times). However, you may not engage in multiple transactions at the same time (ie, you must sell the stock before you buy again).

## Solution

貪心算法，只要當天比昨天的貴，即有錢賺，就買買買！

```c
int maxProfit(int *a, int n)
{
	if (a == NULL || n < 1)
		return 0;
	int profit = 0;
	for (int i = 1; i < n; ++i) {
		int diff = a[i] - a[i - 1];
		if (diff > 0)
			profit += diff;
	}
	return profit;
}
```

## 擴展

[Best Time to Buy and Sell Stock](../BestTimetoBuyandSellStock), 只能交易一次,動態規劃
[Best Time to Buy and Sell Stock II](#),能不限次數交易， 貪心算法
[Best Time to Buy and Sell Stock III](../Best Time to Buy and Sell Stock III)
