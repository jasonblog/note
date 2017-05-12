## Reverse Words in a String

Given an input string, reverse the string word by word.

For example,
Given s = "`the sky is blue`",
return "`blue is sky the`".

For C programmers: Try to solve it in-place in O(1) space.


Clarification:
* What constitutes a word?
	A sequence of non-space characters constitutes a word.

* Could the input string contain leading or trailing spaces?
	Yes. However, your reversed string should not contain leading or trailing spaces.

* How about multiple spaces between two words?
	Reduce them to a single space in the reversed string.

## Solution

1. 先压缩字符串,去掉多余空格。
2. reverse整个字符串.
3. reverse每个单词

## Code
```c
void swap(char *a, char *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}
void reverse(char *str, int s, int t)
{
	int i = s, j = t;
	while (i < j)
		swap(&str[i++], &str[j--]);
}
char *compress(char *s)
{
	if (s == NULL)
		return NULL;
	int n = strlen(s);
	int i = 0;
	int j = n - 1; 
	while (isblank(s[i])) i++;
	while (isblank(s[j])) j--;
	int k;
	for (k = 0; i <= j; ++i) {
		if (!isblank(s[i]) || (i > 0 && !isblank(s[i - 1]))) {
			s[k++] = s[i];
		}
	}
	s[k] = 0;
	return s;
}
void reverseWords(char *s)
{
	if (s == NULL)
		return;
	compress(s);
	int n = strlen(s);
	if (n <= 1)
		return;
	reverse(s, 0, n - 1);
	int pos = 0;
	for (int i = 0; i < n; ++i) {
		if (isblank(s[i])) {
			reverse(s, pos, i - 1);
			pos = i + 1;
		}
	}
	reverse(s, pos, n - 1); // The last word
}
```
