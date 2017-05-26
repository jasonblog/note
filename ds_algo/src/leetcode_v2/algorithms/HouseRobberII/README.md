## House Robber II

Note: This is an extension of [House Robber](../HouseRobber).

After robbing those houses on that street, the thief has found himself a new place for his thievery so that he will not get too much attention. This time, all houses at this place are arranged in a circle. That means the first house is the neighbor of the last one. Meanwhile, the security system for these houses remain the same as for those in the previous street.

Given a list of non-negative integers representing the amount of money of each house, determine the maximum amount of money you can rob tonight without alerting the police.

Credits:
Special thanks to @Freezen for adding this problem and creating all test cases.

## Solution

分別求包括第一個但不包括最後一個節點、包括最後一個但不包括第一個節點的情況

首先實現沒有環的情況:

```c
int robWithoutCircle(int num[], int n) {
	int df[2];
	int i, result = -1;
	if (n <= 0)
		return 0;
	if (n == 1)
		return num[0];
	df[0] = num[0];
	df[1] = max(df[0], num[1]);
	result = max(df[0], df[1]);
	for (i = 2; i < n; ++i) {
		df[get(i)] = max(df[get(i - 2)] + num[i], df[get(i - 1)]);
		result = max(result, df[get(i)]);
	}
	return result;
}
```

實現帶環的情況:

```c
int rob(int *nums, int n)
{
	if (nums == NULL || n <= 0)
		return 0;
	if (n == 1)
		return nums[0];
	return max(robWithoutCircle(nums, n - 1), robWithoutCircle(nums + 1, n - 1));
}
```

## 擴展

1. [House Robber](../HouseRobber): 線性的情況
2. [House Robber II](../HouseRobberII)：線性的情況，首位相連
3. [House Robber III](../HouseRobberIII)：樹形結構
