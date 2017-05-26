## Pascal's Triangle II
Given an index k, return the kth row of the Pascal's triangle.

For example, given k = 3,
Return `[1,3,3,1]`.

Note:
Could you optimize your algorithm to use only O(k) extra space? 

## Solution

根據[Pascal's Triangle](../PascalsTriangle)分析，很容易求出第k行.

注意題目k從0開始計算，因此第k行有k + 1個數。

使用迭代的方式求：後面的一個數等於前面的數乘以一個分數,這個分數從左到右分別為n/1, (n-1)/2, ..., 2/(n-1), 1/n

比如求第5行:

* 初始化第一項為1
* 分數從左到右為`5/1, 4/2， 3/3, 2/4, 1/5`， 因此從左到右的數字為`1 * 5/1 == 5, 5 * 4/2 == 10, 10 * 3/3 = 10, 10 * 2/4 == 5, 5 * 1/5 == 1`, 最後結果為`1,5,10,10,5,1`

因此代碼很容易為:

```cpp
vector<int> getRow(int n) {
	vector<int> result(n + 1, 0);
	result[0] = 1;
	for (int i = 1; i <= n; ++i) {
		result[i] = result[i - 1] * (n - i + 1) / i;
	}
	return result;
}
```

但注意當前一個數乘以分子時可能會溢出，因此先需要約分。。

約分需要求最大公約數。如何求兩個數的最大公約數？

最簡單的方法是使用輾轉相除法，即

```c
int gcd(int x, int y)
{
	return (!y) ? x : gcd(y, x % y);
}
```

但我們使用了模運算，開銷大。

我們使用類似輾轉相除法分析，如果一個數能夠同時整除x和y，則必能同時整除x-y和y，而能夠同時整除x-y和y也能夠整除x和y，即x和y的公約數與x-y和y的公約數相同。即gcd(x, y) = gcd(x - y, y).

```c
int gcd(int x, int y)
{
	if (x < y)
		return gcd(y, x);
	if (y == 0)
		return x;
	return gcd(x - y, y);
}
```

但是雖然使用減法代替了除法運算，但是減法的迭代次數太多了。

從分析公約數的特定入手:

* 對於Y和X，若`Y = k * x, X = k * x`, 則`gcd(Y, X) = k * gcd(y, x)`
* 若`X = p * x`, 假設p是素數， 並且`Y % p != 0`(y不能整除p), 則`gcd(X, Y) = gcd(p * x, Y) = gcd(x, Y)`
* 同理當`Y = p * y`的情況

注意到以上三點後，就可以大幅度對算法改進。

最簡單的方法是，2是素數，同時除以2可以使用移位操作，因此我們可以取p = 2

* 若x,y均是偶數, `gcd(x, y) = gcd(x >> 1, y >> 1) << 1`
* 若x為偶數，y為奇數, `gcd(x, y) = gcd(x >> 1, y)`
* 若y為偶數，x為奇數, `gcd(x, y) = gcd(x, y >> 1)`
* 若x，y都是奇數, `gcd(x, y) = gcd(y, x - y)`

最壞情況下時間複雜度`O(log(max(x, y)))`

```c
int gcd(int a, int b)
{
	if (a < b)
		return gcd(b, a);
	if (b == 0)
		return a;
	if ((a & 0x1) == 0) {
		if ((b & 0x1) == 0)
			return gcd(a >> 1, b >> 1) << 1;
		else
			return gcd(a >> 1, b);
	} else {
		if ((b & 0x1) == 0)
			return gcd(a, b >> 1);
		else
			return gcd(b, a - b);
	}
}
```

最後代碼為:

```cpp
vector<int> getRow(int n) {
	vector<int> result(n + 1, 0);
	result[0] = 1;
	for (int i = 1; i <= n; ++i) { // 必須約分， 否則溢出
		int molecular = (n - i + 1), denominator = i;
		int g = gcd(molecular, denominator);
		molecular /= g;
		denominator /= g;
		int multiplier = result[i - 1];
		g = gcd(multiplier, denominator);
		multiplier /= g;
		denominator /= g;
		int value = multiplier * molecular / denominator;
		result[i] = value;
	}
	return result;
}
```

## 擴展

關於楊輝三角性質[Pascal's Triangle](../PascalsTriangle)
