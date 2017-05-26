## Single Number

Given an array of integers, every element appears twice except for one. Find that single one.

Note:
Your algorithm should have a linear runtime complexity. Could you implement it without using extra memory? 

## Solution

使用異或位運算，相同的數字必然相互抵消，剩下只有一個數字的

## Code
```c
int singleNumber(int a[], int n)
{
	int result = 0;
	for (int i = 0; i < n; ++i)
		result ^= a[i];
	return result;
}
```
