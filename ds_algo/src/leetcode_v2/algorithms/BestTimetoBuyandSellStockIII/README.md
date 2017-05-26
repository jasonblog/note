## Best Time to Buy and Sell Stock III

Say you have an array for which the ith element is the price of a given stock on day i.

Design an algorithm to find the maximum profit. You may complete at most two transactions.

Note:
You may not engage in multiple transactions at the same time (ie, you must sell the stock before you buy again).

## Solution

很容易想到的是，分隔數組成兩個部分，然後分別求最大數對之差，最大利潤就是兩邊和的最大值

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

時間複雜度是O(n<sup>2</sup>), 因此TLE！

這裡的主要瓶頸在於每次都要分別對左右兩部分求最大數對之差，時間複雜度是O(n), 其實我們可以預先保存值，只需要O(1)的時間複雜度。

首先我們先看看如何求最大數對之差，[Best Time to Buy and Sell Stock](../BestTimetoBuyandSellStock)中使用了左掃描法求解最大數對之差。即

遍歷數組，同時保存掃描的最小值，則當前值減去最小值可能成為解（局部最優解).

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

其實我們還可以使用向右掃描法，此時保存最大值，則最大值減去當前值可能成為解(局部最優解).

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

綜合以上兩個方法，當我們在i點切割數組時，如果我們知道i之前的最小值，並且知道i之後的最大值，則使用O(1)的時間可以求出左右兩部分的最大數對之差.

因此我們可以使用兩個數組分別保存i之前的最小值和i之後的最大值.

保存i之前的最小值

```c
leftMin[0] = prices[0];
for (int i = 1; i < n; ++i) {
    leftMin[i] = min(leftMin[i - 1], prices[i]);
}
```

保存i之後的最大值

```c
rightMax[0] = prices[n - 1];
for (int i = n - 2, k = 1; i >=0; --i, k++)
	rightMax[k] = max(rightMax[k - 1], prices[i]);
```

然後切割數組，設left和right分別表示左邊的最大數對之差和右邊的最大數對之差，則

```c
left = max(left, prices[i] - leftMin[i]);
right = rightMax[n - i - 1] - prices[i];
```

`left = max(left, prices[i] - leftMin[i])`保證了左邊一定是最優解。但右邊沒有使用max，即右邊不是最優解，因為當i向後移動時，可能
會消解掉局部最優解，如`0 9 1 6`, 當i = 0，時最優解是9，當i等於1時，此時子數組為`9 1 6`, 最大值為5. 

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

## 擴展

* [Best Time to Buy and Sell Stock](../BestTimetoBuyandSellStock)
* [Best Time to Buy and Sell Stock II](../BestTimetoBuyandSellStockII)
* [Best Time to Buy and Sell Stock III](../BestTimetoBuyandSellStockIII)
