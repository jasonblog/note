## Counting Bits

Given a non negative integer number num. For every numbers i in the range 0 ≤ i ≤ num calculate the number of 1's in their binary representation and return them as an array.

Example:
For `num = 5` you should return `[0,1,1,2,1,2]`.

Follow up:

* It is very easy to come up with a solution with run time O(n\*sizeof(integer)). But can you do it in linear time O(n) /possibly in a single pass?
* Space complexity should be O(n).
* Can you do it like a boss? Do it without using any builtin function like\_\_builtin\_popcount in c++ or in any other language.

## Solution

很容易實現O(n * sizeof(num))的方法：

```c
int countBit(int n)
{
	int sum = 0;
	while (n) {
		sum++;
		n &= (n - 1);
	}
	return sum;
}
int *countBits(int num, int *size)
{
	*size = num + 1;
	int* ans = (int *)malloc(sizeof(int) * *size);
	for (int i = 0; i <= num; ++i)
		ans[i] = countBit(i);
	return ans;
}
```

但題目要求O(n),我們可以使用迭代的方法，我們知道左移位除了最後一個1可能丟掉，不會改變其餘位的1的個數，假設f(n)表示n的二進制1的個數，則：

* 若n為偶數，則f(n) = f(n >> 1).
* 若n為奇數，則f(n) = f (n >> 1) + 1. 其中後面的1是由於移位丟掉的。

```c
int *countBits(int num, int *size)
{
	*size = num + 1;
	int* ans = (int *)malloc(sizeof(int) * *size);
	for (int i = 1; i <= num; ++i)
		ans[i] = ans[i >> 1] + (i & 1);
	return ans;
}
```
