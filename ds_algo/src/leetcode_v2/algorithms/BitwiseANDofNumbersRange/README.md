## Bitwise AND of Numbers Range

Given a range [m, n] where 0 <= m <= n <= 2147483647, return the bitwise AND of all numbers in this range, inclusive.

For example, given the range [5, 7], you should return 4. 

## Solution 1

直接暴力求解。因為`n & n -1`就是把把n的最後一個1清空，並且只要其中有一個為0，`&`操作就為0，於是我們從後面開始計算，遇到`2^n & 2^n - 1`必然會提前退出

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

結合上次的思路，`2^n & 2^n - 1`等於0，設N為n的最高位為1的數，比如`n = 7， N = 4， n = 8, N = 8, n = 11, N = 8`, 則若`m < N`, 直接返回0

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

上面解法只是過濾了當值等於0的情況，而結果不等於0時，並不能減少計算量

## Solution 3

我們取m，n二進制的公共前綴，後面添加0直到與n等長，比如`n = 101011`, `m = 101000`, 則為`101000`,後面部分為`00~11`,相與必然為0, 因此101000就是最後的結果

因此此題轉化成求n,m的二進制公共前綴.

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

循環至多32次，大大減少了計算量
