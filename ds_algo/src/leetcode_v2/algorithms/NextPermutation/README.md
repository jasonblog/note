## Next Permutation

Implement next permutation, which rearranges numbers into the lexicographically next greater permutation of numbers.

If such arrangement is not possible, it must rearrange it as the lowest possible order (ie, sorted in ascending order).

The replacement must be in-place, do not allocate extra memory.

Here are some examples. Inputs are in the left-hand column and its corresponding outputs are in the right-hand column.
```
1,2,3 → 1,3,2
3,2,1 → 1,2,3
1,1,5 → 1,5,1
```

## Solution

算法见[July字符串全排列](https://github.com/julycoding/The-Art-Of-Programming-By-July/blob/master/ebook/zh/01.06.md)

这里需要注意的是有相同的数字出现，注意循环的边界条件

## Code
```c
void nextPermutation(int num[], int n)
{
	int i, j;
	for (i = n - 2; i >= 0 && num[i] >= num[i + 1]; --i); // 注意边界条件num[i] >= num[i + 1]
	if (i < 0) {
		reverse(num, 0, n - 1);
		return;
	}
	for (j = n - 1; j > i && num[j] <= num[i]; --j); // 注意边界条件num[j] <= num[i];
	swap(&num[i], &num[j]);
	reverse(num, i + 1, n - 1);
}
```
