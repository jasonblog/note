## Sqrt(x)

Implement `int sqrt(int x)`.

Compute and return the square root of x.

## Solution

可以直接从i开始遍历到x/2,直到 i * i > x为止

**注意，i * i 有可能会溢出，因此和i > x / i比较.**
```c
int sqrt(int x)
{
	if (x == 0)
		return 0;
	int i;
	for (i = 1; i < x / 2; ++i)
		if (i > x / i)
			break;
	return i - 1;
}
```

上面这个算法复杂度是O(n / 2),即O(n).

我们可以使用二分法。

不过还是需要注意不能用\*求平方，否则会溢出，另外求中间值时也不能用`(left + right) / 2`，因为`left + right`也有可能溢出.

同样注意不能这样`left/2 + right / 2`,因此这样的话`1 / 2 + 3 / 2 == 1`,而中间值明显是2

应该使用`left + (right - left) / 2`

另外注意当`mid == left`, 应该返回left.
```c
int sqrt(int x) {
	if (x == 0)
		return 0;
	int left = 1, right = x;
	int ans = 0;;
	while (left < right) {
		//int mid = (left >> 1) + (right >> 1); 1/2 + 1/2 = 0
		//int mid = (left + right) >> 1; overflow
		int mid = left + ((right - left) >> 1);
		if (mid == left || mid == x / mid) // 这个条件需要注意
			return mid;
		if (mid < x / mid) {
			left = mid;
		} else {
			right = mid;
		}

	}
	return left;
}
```


上面这个方法把复杂度降低到了O(logn).

## O(1)算法

这个更高级的，使用位运算。。

```c
int sqrt(int x)
{
	long ans = 0;
	long bit = 1L << 16;
	while (bit) {
		ans |= bit;
		if (ans * ans > x)
			ans ^= bit;
		bit >>= 1;
	}
	return ans;
}
```
一位一位的试，大于就把该位置0，否则保留1， `ans ^= bit`就是把该位重新置0， `bit = 1L << 16`是因为`int`范围内的`sqrt`值不会超过2<sup>16</sup>
