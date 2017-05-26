## Edit Distance

Given two words word1 and word2, find the minimum number of steps required to convert word1 to word2. (each operation is counted as 1 step.)

You have the following 3 operations permitted on a word:
```
	a) Insert a character
	b) Delete a character
	c) Replace a character
```

## Solution

首先word1變成word2和word2變成word1的步數一樣的.

假設需要把s[1..i]變成t[1..j],例如將beauty變成batyu, 所需要的最少的操作數.我們可能有3種可能:

1. 把s[1..i] 轉化成t[1..j-1], 設操作次數為case1,我們只需要把t[j]加到後面即可，因此總操作次數為case1 + 1. **添加操作** `比如ab abc`
2. 把s[1..i-1] 轉化成t[1..j], 設操作數為case2， 我們只需要把s[i]從後面刪除即可，總操作次數為case2 + 1. ** 刪除操作 ** `比如abc ab`
3. 如果我們可以把s[1..i-1]轉化成t[1..j-1],設操作數為case3，如果s[i] == s[j],總操作數為case3， 否則需要替換一個字符，總操作數為case3 + 1. **替換操作** 比如`abc abe`

我們取這三種情況的最小值作為最後的結果。

我們使用`dp[i][j]`表示s[1..i]轉化成t[1..j]的最小操作數。顯然:

* `dp[0][j] == j`, 只需要把t全部刪除即可
* 同理`dp[i][0] == i`

因此該題轉化成dp問題

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
