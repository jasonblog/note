# House Robber

## Problem
You are a professional robber planning to rob houses along a street. Each house has a certain amount of money stashed, the only constraint stopping you from robbing each of them is that adjacent houses have security system connected and it will automatically contact the police if two adjacent houses were broken into on the same night.

Given a list of non-negative integers representing the amount of money of each house, determine the maximum amount of money you can rob tonight without alerting the police.

## Solution
题目的意思很明了，就是给定一个正数数组，求数组和的最大值，唯一的约束条件是相邻的两个数不能相加。

比如8 2 2 9 ，最大和为17， 中间2 2 间隔。

很明显这是一个简单的动态规划问题。需要明白的是，数字之间相加中间至少间隔一个数，至多两个数，不会出现间隔2个以上的情况，因为如果间隔3个数，那最大值一定可以加上中间的一个数，并且大于当前最大值，矛盾。

显然动态方程df(i) = max(df[i - 2] + num[i], df[i - 1])

于是我们可以定义一个df数组保存局部最优解。但是我们发现df(i)只依赖于df(i - 1),df(i - 2)，和前面的值无关，
于是我们只需要保存前两项即可，利用**滚动数组**解决。

因此最后的时间复杂度是O（n），空间复杂度是O（1）
代码为：
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

## 扩展

1. [House Robber](../HouseRobber): 线性的情况
2. [House Robber II](../HouseRobberII)：线性的情况，首位相连
3. [House Robber III](../HouseRobberIII)：树形结构
