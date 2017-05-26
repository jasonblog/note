## Sqrt(x)

Implement `int sqrt(int x)`.

Compute and return the square root of x.

## Solution

可以直接從i開始遍歷到x/2,直到 i * i > x為止

**注意，i * i 有可能會溢出，因此和i > x / i比較.**
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

上面這個算法複雜度是O(n / 2),即O(n).

我們可以使用二分法。

不過還是需要注意不能用\*求平方，否則會溢出，另外求中間值時也不能用`(left + right) / 2`，因為`left + right`也有可能溢出.

同樣注意不能這樣`left/2 + right / 2`,因此這樣的話`1 / 2 + 3 / 2 == 1`,而中間值明顯是2

應該使用`left + (right - left) / 2`

另外注意當`mid == left`, 應該返回left.
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
		if (mid == left || mid == x / mid) // 這個條件需要注意
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


上面這個方法把複雜度降低到了O(logn).

## O(1)算法

這個更高級的，使用位運算。。

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
一位一位的試，大於就把該位置0，否則保留1， `ans ^= bit`就是把該位重新置0， `bit = 1L << 16`是因為`int`範圍內的`sqrt`值不會超過2<sup>16</sup>
