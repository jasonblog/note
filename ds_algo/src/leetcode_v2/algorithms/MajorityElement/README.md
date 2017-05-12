## Majority Element

Given an array of size n, find the majority element. The majority element is the element that appears more than ⌊ n/2 ⌋ times.

You may assume that the array is non-empty and the majority element always exist in the array.

## Solution

一个数字出现数组次数的一半，即这个数字的次数超过其他数字出现次数之和

可以用一个count指针，以及当前候选数字result:

* 若`a[i] == result`, 则`count++`，否则若`a[i] != result`,则`count--`
* 若`count == 0`, 则更新result，即`result = a[i]`

最后result就是次数超过一半的数字

## Code
```c
int majorityElement(int a[], int n)
{
	if (n < 2)
		return a[0];
	int result = a[0];
	int count = 0;
	for (int i = 0; i < n; ++i) {
		if (a[i] == result) {
			++count;
		} else {
			--count;
		}
		if (count == 0) {
			count = 1;
			result = a[i];
		}
	}
	return result;
}
```
