## Power of Two

Given an integer, write a function to determine if it is a power of two.

Credits:
Special thanks to @jianchao.li.fighter for adding this problem and creating all test cases.

## Solution

位运算，一个数是2的幂，则它的二进制1的个数等于1

求一个数二进制的1的个数的方法是不断地`n &= (n - 1)`，直到n为0

考虑以下两种情况:

* `n == 0`， 此时`n & (n - 1) == 0`, 但不是2的幂
* `n=-2147483648`，此时二进制1的个数也等于1，但它不是2的幂

综合可以得出：只要`n <= 0`, 就一定不是2的幂。

因此最后解决方案为:

```c
bool isPowerOfTwo(int n)
{
	if (n <= 0)
		return false;
	return !(n & (n - 1));
}
```
