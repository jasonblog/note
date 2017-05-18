## Pascal's Triangle II
Given an index k, return the kth row of the Pascal's triangle.

For example, given k = 3,
Return `[1,3,3,1]`.

Note:
Could you optimize your algorithm to use only O(k) extra space? 

## Solution

根据[Pascal's Triangle](../PascalsTriangle)分析，很容易求出第k行.

注意题目k从0开始计算，因此第k行有k + 1个数。

使用迭代的方式求：后面的一个数等于前面的数乘以一个分数,这个分数从左到右分别为n/1, (n-1)/2, ..., 2/(n-1), 1/n

比如求第5行:

* 初始化第一项为1
* 分数从左到右为`5/1, 4/2， 3/3, 2/4, 1/5`， 因此从左到右的数字为`1 * 5/1 == 5, 5 * 4/2 == 10, 10 * 3/3 = 10, 10 * 2/4 == 5, 5 * 1/5 == 1`, 最后结果为`1,5,10,10,5,1`

因此代码很容易为:

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

但注意当前一个数乘以分子时可能会溢出，因此先需要约分。。

约分需要求最大公约数。如何求两个数的最大公约数？

最简单的方法是使用辗转相除法，即

```c
int gcd(int x, int y)
{
	return (!y) ? x : gcd(y, x % y);
}
```

但我们使用了模运算，开销大。

我们使用类似辗转相除法分析，如果一个数能够同时整除x和y，则必能同时整除x-y和y，而能够同时整除x-y和y也能够整除x和y，即x和y的公约数与x-y和y的公约数相同。即gcd(x, y) = gcd(x - y, y).

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

但是虽然使用减法代替了除法运算，但是减法的迭代次数太多了。

从分析公约数的特定入手:

* 对于Y和X，若`Y = k * x, X = k * x`, 则`gcd(Y, X) = k * gcd(y, x)`
* 若`X = p * x`, 假设p是素数， 并且`Y % p != 0`(y不能整除p), 则`gcd(X, Y) = gcd(p * x, Y) = gcd(x, Y)`
* 同理当`Y = p * y`的情况

注意到以上三点后，就可以大幅度对算法改进。

最简单的方法是，2是素数，同时除以2可以使用移位操作，因此我们可以取p = 2

* 若x,y均是偶数, `gcd(x, y) = gcd(x >> 1, y >> 1) << 1`
* 若x为偶数，y为奇数, `gcd(x, y) = gcd(x >> 1, y)`
* 若y为偶数，x为奇数, `gcd(x, y) = gcd(x, y >> 1)`
* 若x，y都是奇数, `gcd(x, y) = gcd(y, x - y)`

最坏情况下时间复杂度`O(log(max(x, y)))`

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

最后代码为:

```cpp
vector<int> getRow(int n) {
	vector<int> result(n + 1, 0);
	result[0] = 1;
	for (int i = 1; i <= n; ++i) { // 必须约分， 否则溢出
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

## 扩展

关于杨辉三角性质[Pascal's Triangle](../PascalsTriangle)
