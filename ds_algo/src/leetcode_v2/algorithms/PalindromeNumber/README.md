## Palindrome Number 

Determine whether an integer is a palindrome. Do this without extra space.

click to show spoilers.

Some hints:

* Could negative integers be palindromes? (ie, -1)

* If you are thinking of converting the integer to string, note the restriction of using extra space.

* You could also try reversing an integer. However, if you have solved the problem "Reverse Integer", you know that the reversed integer might overflow. How would you handle such case?

**There is a more generic way of solving this problem.**

## Solution

提示已经说了, 可以转化成字符串，即使用`sprintf(buf, "%d", x)`，然后判断，但需要额外空间

或者把的各位逆转，比如`123 -> 321`，然后判断二者是否相等，但这样需要考虑是否有溢出情况

其实可以直接判断，即从最高位、最低位逐一比较，比如1234321,最高位从百万位的1开始向右扫描，最低位从个位的1开始向左扫描，
逐一判断是否相等。

关键在于如何x取得各位，即最高位high和最低位low。设x=1234321

1. 最低位容易`low = x % 10 = 1234321 % 10 = 1`;
2. 最高位就是除以N = 10<sup>n-1</sup>,n为x的位数,等于7, 即`high = x / 1000000 = 1`;
3. 显然若`low != high` 返回`false`，否则`low == high`, 此时要把x的最高位和最低位去掉，得到23432.方法是：去掉最高位x %= N, 然后去掉最低位 x /= 10.
4. 回到1,直到x=0.

## Code
```c
bool isPalindrome(int x)
{
	if (x < 0)
		return false;
	int len = 1;
	for (; x / len >= 10; len *= 10);
	while (x) {
		int high = x / len;
		int low = x % 10;
		if (low != high)
			return false;
		x = (x % len) / 10;
		len /= 100;
	}
	return true;
}
```
