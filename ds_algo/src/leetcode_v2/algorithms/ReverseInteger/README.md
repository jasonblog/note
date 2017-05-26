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

如果沒有提示，很容易寫出代碼：
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
但是這沒有考慮溢出。這道題很簡單，主要難點就是在於溢出問題。

需要考慮溢出，必須保證在結果還沒有溢出之前就處理完畢。

其基本思路是每次和`INT_MAX / 10`作比較，如果大於等於`INT_MAX / 10`，則
退出循環，此時x也一定到了最後一位(因為x也最多32位), 即x一定小於10。

假設原來的數是n, ans由於大於等於`INT_MAX / 10`而退出循環，x為最後的值。
此時需要處理邊界問題。

* 如果是負數, 最小值為-2147483648
* 如果是正數, 最大值是2147483647
* 若`ans > INT_MAX / 10`，則一定會溢出
* 若`ans == INT_MAX / 10`, 若`n < 0, 且 x <= 8`,不會溢出。`n > 0 && x <= 7`不會溢出.否則也會溢出

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
	if (ans > BOUND) // 此時乘以10,無論如何都要溢出
		return 0;
	if (sign && x == 8) // 剛好是最大的負數，其實這種情況不可能出現
		return -2147483648;
	if (x > 7) // ans已經等於INT_MAX / 10，如果個位大於7溢出.
		return 0;
	ans *= 10;
	ans += x; // 此時一定是2147483641
	return sign ? -ans : ans;
}
```
