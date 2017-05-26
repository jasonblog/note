## Power of Two

Given an integer, write a function to determine if it is a power of two.

Credits:
Special thanks to @jianchao.li.fighter for adding this problem and creating all test cases.

## Solution

位運算，一個數是2的冪，則它的二進制1的個數等於1

求一個數二進制的1的個數的方法是不斷地`n &= (n - 1)`，直到n為0

考慮以下兩種情況:

* `n == 0`， 此時`n & (n - 1) == 0`, 但不是2的冪
* `n=-2147483648`，此時二進制1的個數也等於1，但它不是2的冪

綜合可以得出：只要`n <= 0`, 就一定不是2的冪。

因此最後解決方案為:

```c
bool isPowerOfTwo(int n)
{
	if (n <= 0)
		return false;
	return !(n & (n - 1));
}
```
