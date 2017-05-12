## Reverse Integer

Reverse digits of an integer.
```
Example1: x = 123, return 321
Example2: x = -123, return -321
```
click to show spoilers.

## Have you thought about this?

Here are some good questions to ask before coding. Bonus points for you if you have already thought through this!

* If the integer's last digit is 0, what should the output be? ie, cases such as 10, 100.

* Did you notice that the reversed integer might overflow? Assume the input is a 32-bit integer, then the reverse of 1000000003 overflows. How should you handle such cases?

For the purpose of this problem, assume that your function returns 0 when the reversed integer overflows.

Update (2014-11-10):
Test cases had been added to test the overflow behavior.

## Solution

如果没有提示，很容易写出代码：
```c
int reverse(int x)
{
	int sign = 0, ans = 0;
	if (x < 0) {
		x = -x;
		sign = 1;
	}
	while (x) {
		ans *= 10;
		ans += x % 10;
		x /= 10;
	}
	return sign ? -ans : ans;
}
```
但是这没有考虑溢出。这道题很简单，主要难点就是在于溢出问题。

需要考虑溢出，必须保证在结果还没有溢出之前就处理完毕。

其基本思路是每次和`INT_MAX / 10`作比较，如果大于等于`INT_MAX / 10`，则
退出循环，此时x也一定到了最后一位(因为x也最多32位), 即x一定小于10。

假设原来的数是n, ans由于大于等于`INT_MAX / 10`而退出循环，x为最后的值。
此时需要处理边界问题。

* 如果是负数, 最小值为-2147483648
* 如果是正数, 最大值是2147483647
* 若`ans > INT_MAX / 10`，则一定会溢出
* 若`ans == INT_MAX / 10`, 若`n < 0, 且 x <= 8`,不会溢出。`n > 0 && x <= 7`不会溢出.否则也会溢出

## Code
```c
int reverse(int x)
{
	int sign = 0, ans = 0;
	const int BOUND = INT_MAX / 10;
	if (x == -2147483648)
		return 0;
	if (x < 0) {
		x = -x;
		sign = 1;
	}
	while (x) {
		ans *= 10;
		ans += x % 10;
		x /= 10;
		if (ans >= BOUND)
			break;
	}
	if (x == 0)
		return sign ? -ans : ans;
	if (ans > BOUND) // 此时乘以10,无论如何都要溢出
		return 0;
	if (sign && x == 8) // 刚好是最大的负数，其实这种情况不可能出现
		return -2147483648;
	if (x > 7) // ans已经等于INT_MAX / 10，如果个位大于7溢出.
		return 0;
	ans *= 10;
	ans += x; // 此时一定是2147483641
	return sign ? -ans : ans;
}
```
