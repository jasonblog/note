## Bitwise AND of Numbers Range

Given a range [m, n] where 0 <= m <= n <= 2147483647, return the bitwise AND of all numbers in this range, inclusive.

For example, given the range [5, 7], you should return 4. 

## Solution 1

直接暴力求解。因为`n & n -1`就是把把n的最后一个1清空，并且只要其中有一个为0，`&`操作就为0，于是我们从后面开始计算，遇到`2^n & 2^n - 1`必然会提前退出

## Code
```
int slowRangeBitwiseAnd(int m, int n) {
	int ans = n;
	for (int i = n - 1; i >= m && ans; --i)
		ans &= i;
	return ans;
}
```

## Solution 2

结合上次的思路，`2^n & 2^n - 1`等于0，设N为n的最高位为1的数，比如`n = 7， N = 4， n = 8, N = 8, n = 11, N = 8`, 则若`m < N`, 直接返回0

## Code
```c
int midRangeBitwiseAnd(int m, int n)
{
	int base = 1;
	int t = n;
	while ((t >> 1)) {
		base <<= 1;
		t >>= 1;
	}
	if (m < base)
		return 0;
	int ans = n;
	for (int i = n - 1; i >= m; --i)
		ans &= i;
	return ans;
}
```

上面解法只是过滤了当值等于0的情况，而结果不等于0时，并不能减少计算量

## Solution 3

我们取m，n二进制的公共前缀，后面添加0直到与n等长，比如`n = 101011`, `m = 101000`, 则为`101000`,后面部分为`00~11`,相与必然为0, 因此101000就是最后的结果

因此此题转化成求n,m的二进制公共前缀.

## Code
```c
int rangeBitwiseAnd(int m, int n) {
	if (m == n)
		return m;
	if (m == 0)
		return 0;
	int base = 1;
	int t = n;
	while ((t >> 1)) {
		base <<= 1;
		t >>= 1;
	}
	int ans = 0;
	while (base) {
		int a = base & m;
		int b = base & n;
		if (a != b)
			return ans;
		ans |= a;
		base >>= 1;
	}
	return ans;
}
```

循环至多32次，大大减少了计算量
