## Length of Last Word 

Given a string s consists of upper/lower-case alphabets and empty space characters ' ', return the length of last word in the string.

If the last word does not exist, return 0.

Note: A word is defined as a character sequence consists of non-space characters only.

```
For example, 
Given s = "Hello World",
return 5.
```

## Solution

先去掉后面的空格字符，然后从后面开始扫描直到遇到空格为止

## Code
```c
int lengthOfLastWord(const char *s)
{
	if (!s || !*s) return 0;
	int len = strlen(s);
	int sum = 0;
	int i = len - 1;
	while (i >= 0 && !isalpha(s[i])) i--;
	while (isalpha(s[i])) {
		++sum;
		i--;
	}
	return sum;
}
```
