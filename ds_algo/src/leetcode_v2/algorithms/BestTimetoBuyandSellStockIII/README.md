## Best Time to Buy and Sell Stock III

Say you have an array for which the ith element is the price of a given stock on day i.

Design an algorithm to find the maximum profit. You may complete at most two transactions.

Note:
You may not engage in multiple transactions at the same time (ie, you must sell the stock before you buy again).

## Solution

很容易想到的是，分隔数组成两个部分，然后分别求最大数对之差，最大利润就是两边和的最大值

```cpp
int maxProfit(vector<int>& prices) {
    int n = prices.size();
    if (n <= 1)
	    return 0;
    int profit = 0;
    for (int i = 0; i < prices.size(); ++i) {
	    int p = maxProfit(prices, 0, i) + maxProfit(prices, i, n - 1);
	    profit = max(profit, p);
    }
    return profit;
}
int maxProfit(vector<int>& a, int s, int t) {
    if (s >= t)
	    return 0;
    int profit = 0;
    int minPrice = a[s];
    for (int i = s + 1; i <= t; ++i) {
	    minPrice = min(minPrice, a[i]);
	    profit = max(profit, a[i] - minPrice);
    }
    return profit;
}
```

时间复杂度是O(n<sup>2</sup>), 因此TLE！

这里的主要瓶颈在于每次都要分别对左右两部分求最大数对之差，时间复杂度是O(n), 其实我们可以预先保存值，只需要O(1)的时间复杂度。

首先我们先看看如何求最大数对之差，[Best Time to Buy and Sell Stock](../BestTimetoBuyandSellStock)中使用了左扫描法求解最大数对之差。即

遍历数组，同时保存扫描的最小值，则当前值减去最小值可能成为解（局部最优解).

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

其实我们还可以使用向右扫描法，此时保存最大值，则最大值减去当前值可能成为解(局部最优解).

```c
int maxProfit(int *a, int n)
{
	int max = a[n - 1];
	int profit = 0;
	for (int i = n - 2; i >= 0; --i) {
		max = MAX(max, a[i]);
		profit = MAX(profit, max - a[i]);
	}
	return profit;
}
```

综合以上两个方法，当我们在i点切割数组时，如果我们知道i之前的最小值，并且知道i之后的最大值，则使用O(1)的时间可以求出左右两部分的最大数对之差.

因此我们可以使用两个数组分别保存i之前的最小值和i之后的最大值.

保存i之前的最小值

```c
leftMin[0] = prices[0];
for (int i = 1; i < n; ++i) {
    leftMin[i] = min(leftMin[i - 1], prices[i]);
}
```

保存i之后的最大值

```c
rightMax[0] = prices[n - 1];
for (int i = n - 2, k = 1; i >=0; --i, k++)
	rightMax[k] = max(rightMax[k - 1], prices[i]);
```

然后切割数组，设left和right分别表示左边的最大数对之差和右边的最大数对之差，则

```c
left = max(left, prices[i] - leftMin[i]);
right = rightMax[n - i - 1] - prices[i];
```

`left = max(left, prices[i] - leftMin[i])`保证了左边一定是最优解。但右边没有使用max，即右边不是最优解，因为当i向后移动时，可能
会消解掉局部最优解，如`0 9 1 6`, 当i = 0，时最优解是9，当i等于1时，此时子数组为`9 1 6`, 最大值为5. 

```cpp
int maxProfit(vector<int>& prices) {
    int n = prices.size();
    if (n <= 1)
	    return 0;
    vector<int> leftMin(n, 0);
    leftMin[0] = prices[0];
    for (int i = 1; i < n; ++i) {
	    leftMin[i] = min(leftMin[i - 1], prices[i]);
    }
    vector<int> rightMax(n, 0);
    rightMax[0] = prices[n - 1];
    for (int i = n - 2, k = 1; i >=0; --i, k++)
	    rightMax[k] = max(rightMax[k - 1], prices[i]);
    int left = 0, right = 0;
    int profit = 0;
    for (int i = 0; i < n; ++i) {
	    left = max(left, prices[i] - leftMin[i]);
	    right = rightMax[n - i - 1] - prices[i];
	    profit = max(profit, left + right);
    }
    return profit;
}
```

## 扩展

* [Best Time to Buy and Sell Stock](../BestTimetoBuyandSellStock)
* [Best Time to Buy and Sell Stock II](../BestTimetoBuyandSellStockII)
* [Best Time to Buy and Sell Stock III](../BestTimetoBuyandSellStockIII)
