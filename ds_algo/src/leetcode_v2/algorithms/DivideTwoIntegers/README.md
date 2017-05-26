## Divide Two Integers

Divide two integers without using multiplication, division and mod operator.

If it is overflow, return `MAX_INT`.

## Solution

不能用/ * % 做除法運算，於是自然想到使用減法.

但是減法運算太慢了，每次只能減一個除數.

比如`100 / 25 = 100 - 25 - 25 - 25 - 25`，於是`100 / 25 = 4`.

很顯然100能夠直接減去`25 * 4 == 100`, 而不能使用`*`,但可以想到使用左移運算符達到效果.

於是我們儘可能每次減得更大，而減數必須是除數的2的冪倍數,比如`125 / 5`, 5的2次冪倍數關係為`5 10 20 40 80 160`.
則`125 = 125 - 80 - 40 - 5`.

於是每次可以遍歷能夠減去的最大數，直到被減數小於減數。


為了處理負數問題，我們可以先判斷出符號，然後取絕對值，當做正數處理.

但有一種情況需要格外處理, 即`INT_MIN`，如果取絕對值，就會溢出，這種情況使用`int`沒法解決，比如`INT_MIN / -2 `.
為了處理這種情況，需要把`int`類型轉化為`long`,即可避免這種情況.

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
	if (divisor == 0) {  // 除數為0
		return dividend >= 0 ? INT_MAX : INT_MIN;
	}
	long n = dividend; // 防止溢出，當其中一個=INT_MIN
	long m = divisor;
	int sign = getSign(n, m); // 保存結果的符號
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
