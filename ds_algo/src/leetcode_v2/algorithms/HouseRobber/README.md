# House Robber

## Problem
You are a professional robber planning to rob houses along a street. Each house has a certain amount of money stashed, the only constraint stopping you from robbing each of them is that adjacent houses have security system connected and it will automatically contact the police if two adjacent houses were broken into on the same night.

Given a list of non-negative integers representing the amount of money of each house, determine the maximum amount of money you can rob tonight without alerting the police.

## Solution
題目的意思很明瞭，就是給定一個正數數組，求數組和的最大值，唯一的約束條件是相鄰的兩個數不能相加。

比如8 2 2 9 ，最大和為17， 中間2 2 間隔。

很明顯這是一個簡單的動態規劃問題。需要明白的是，數字之間相加中間至少間隔一個數，至多兩個數，不會出現間隔2個以上的情況，因為如果間隔3個數，那最大值一定可以加上中間的一個數，並且大於當前最大值，矛盾。

顯然動態方程df(i) = max(df[i - 2] + num[i], df[i - 1])

於是我們可以定義一個df數組保存局部最優解。但是我們發現df(i)只依賴於df(i - 1),df(i - 2)，和前面的值無關，
於是我們只需要保存前兩項即可，利用**滾動數組**解決。

因此最後的時間複雜度是O（n），空間複雜度是O（1）
代碼為：
```c
int rob(int num[], int n) {
	int df[2];
	int i, result = -1;
	if (n == 1)
		return num[0];
	df[0] = num[0];
	df[1] = max(df[0], num[1]);
	result = max(df[0], df[1]);
	for (i = 2; i < n; ++i) {
		df[i % 2] = max(df[(i - 2) % 2] + num[i], df[(i - 1) % 2]);
		result = max(result, df[i % 2]);
	}
	return result;
}
```

## 擴展

1. [House Robber](../HouseRobber): 線性的情況
2. [House Robber II](../HouseRobberII)：線性的情況，首位相連
3. [House Robber III](../HouseRobberIII)：樹形結構
