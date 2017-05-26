## Majority Element

Given an array of size n, find the majority element. The majority element is the element that appears more than ⌊ n/2 ⌋ times.

You may assume that the array is non-empty and the majority element always exist in the array.

## Solution

一個數字出現數組次數的一半，即這個數字的次數超過其他數字出現次數之和

可以用一個count指針，以及當前候選數字result:

* 若`a[i] == result`, 則`count++`，否則若`a[i] != result`,則`count--`
* 若`count == 0`, 則更新result，即`result = a[i]`

最後result就是次數超過一半的數字

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
