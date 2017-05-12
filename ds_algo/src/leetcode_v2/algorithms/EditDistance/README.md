## Edit Distance

Given two words word1 and word2, find the minimum number of steps required to convert word1 to word2. (each operation is counted as 1 step.)

You have the following 3 operations permitted on a word:
```
	a) Insert a character
	b) Delete a character
	c) Replace a character
```

## Solution

首先word1变成word2和word2变成word1的步数一样的.

假设需要把s[1..i]变成t[1..j],例如将beauty变成batyu, 所需要的最少的操作数.我们可能有3种可能:

1. 把s[1..i] 转化成t[1..j-1], 设操作次数为case1,我们只需要把t[j]加到后面即可，因此总操作次数为case1 + 1. **添加操作** `比如ab abc`
2. 把s[1..i-1] 转化成t[1..j], 设操作数为case2， 我们只需要把s[i]从后面删除即可，总操作次数为case2 + 1. ** 删除操作 ** `比如abc ab`
3. 如果我们可以把s[1..i-1]转化成t[1..j-1],设操作数为case3，如果s[i] == s[j],总操作数为case3， 否则需要替换一个字符，总操作数为case3 + 1. **替换操作** 比如`abc abe`

我们取这三种情况的最小值作为最后的结果。

我们使用`dp[i][j]`表示s[1..i]转化成t[1..j]的最小操作数。显然:

* `dp[0][j] == j`, 只需要把t全部删除即可
* 同理`dp[i][0] == i`

因此该题转化成dp问题

```c
int minDistance(char *w1, char *w2)
{
	const int len1 = strlen(w1);
	const int len2 = strlen(w2);
	int dp[len1 + 1][len2 + 1];
	dp[0][0] = 0;
	for (int i = 1; i <= len1; ++i)
		dp[i][0] = i;
	for (int j = 1; j <= len2; ++j)
		dp[0][j] = j;
	for (int i = 1; i <= len1; ++i) {
		for (int j = 1; j <= len2; ++j) {
			int case1 = dp[i - 1][j] + 1;
			int case2 = dp[i][j - 1] + 1;
			int case3 = dp[i - 1][j - 1] + (w1[i - 1] == w2[j - 1] ? 0 : 1);
			dp[i][j] = min3(case1, case2, case3);
		}
	}
	return dp[len1][len2];
}
```
