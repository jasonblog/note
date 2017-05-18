## Permutation Sequence

The set [1,2,3,…,n] contains a total of n! unique permutations.

By listing and labeling all of the permutations in order,
We get the following sequence (ie, for n = 3):
```
"123"
"132"
"213"
"231"
"312"
"321"
```
Given n and k, return the kth permutation sequence.

**Note: Given n will be between 1 and 9 inclusive.**

## Solution

求康托展开式，具体算法见[康托展开算法](https://github.com/krystism/algorithms/tree/master/cantor)

由于是求第k个排列，而康托展开求的是根据比它小的数量，因此需要k减一

## Code

```c
char *listRemove(char *s, int i)
{
	int n = strlen(s), j;
	for (j = i; j < n - 1; ++j)
		s[j] = s[j + 1];
	s[j] = 0;
	return s;
}
char *getPermutation(int n, int k) {
	char *ans = malloc(sizeof(char) * (n + 1));
	char *nums = malloc(sizeof(char) * n);
	const int FAC[] = {1,1,2,6,24,120,720,5040,40320,362880};
	for (int i = 0; i < n; ++i)
		nums[i] = '0' + i + 1;
	k--;
	int cur = 0;
	for (int i = n - 1; i > 0; --i) {
		int index = k / FAC[i];
		k %= FAC[i];
		ans[cur++] = nums[index];
		listRemove(nums, index);
	}
	ans[cur++] = nums[0];
	ans[cur] = 0;
	free(nums);
	return ans;
}
```
