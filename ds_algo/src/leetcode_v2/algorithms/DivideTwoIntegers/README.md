## Divide Two Integers

Divide two integers without using multiplication, division and mod operator.

If it is overflow, return `MAX_INT`.

## Solution

不能用/ * % 做除法运算，于是自然想到使用减法.

但是减法运算太慢了，每次只能减一个除数.

比如`100 / 25 = 100 - 25 - 25 - 25 - 25`，于是`100 / 25 = 4`.

很显然100能够直接减去`25 * 4 == 100`, 而不能使用`*`,但可以想到使用左移运算符达到效果.

于是我们尽可能每次减得更大，而减数必须是除数的2的幂倍数,比如`125 / 5`, 5的2次幂倍数关系为`5 10 20 40 80 160`.
则`125 = 125 - 80 - 40 - 5`.

于是每次可以遍历能够减去的最大数，直到被减数小于减数。


为了处理负数问题，我们可以先判断出符号，然后取绝对值，当做正数处理.

但有一种情况需要格外处理, 即`INT_MIN`，如果取绝对值，就会溢出，这种情况使用`int`没法解决，比如`INT_MIN / -2 `.
为了处理这种情况，需要把`int`类型转化为`long`,即可避免这种情况.

## Code
```c
int getSign(long a, long b)
{
	if ((a < 0 && b > 0) || (a > 0 && b < 0))
		return 1;
	return 0;
}
long labs(long a)
{
	return a >= 0 ? a : -a;
}
int divide(int dividend, int divisor)
{
	if (divisor == 0) {  // 除数为0
		return dividend >= 0 ? INT_MAX : INT_MIN;
	}
	long n = dividend; // 防止溢出，当其中一个=INT_MIN
	long m = divisor;
	int sign = getSign(n, m); // 保存结果的符号
	n = labs(n);
	m = labs(m);
	long ans = 0;
	long a; // 防止 a << 1溢出
	long k;
	while (n >= m) {
		a = m, k = 1;
		while (n > (a << 1)) {
			a <<= 1;
			k <<= 1;
		}
		n -= a;
		ans += k;
	}
	ans = sign ? -ans : ans;
	if (ans > INT_MAX)
		return INT_MAX;
	if (ans < INT_MIN)
		return INT_MIN;
	return ans;
}
```
