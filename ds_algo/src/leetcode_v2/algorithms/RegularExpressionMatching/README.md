## Regular Expression Matching 
Implement regular expression matching with support for `'.'` and `'*'`.

`'.'` Matches any single character.
`'*'` Matches zero or more of the preceding element.

The matching should cover the entire input string (not partial).

The function prototype should be:
```
bool isMatch(const char *s, const char *p)
```

Some examples:
```
isMatch("aa","a") → false
isMatch("aa","aa") → true
isMatch("aaa","aa") → false
isMatch("aa", "a*") → true
isMatch("aa", ".*") → true
isMatch("ab", ".*") → true
isMatch("aab", "c*a*b") → true
```

## Solution

如果暴力求解, 則太複雜了。

設當前待匹配字符地址為`s`, 匹配地址為`p`, `p`的下一個地址為`next`, 即`match(s, p)`

分兩種情況:

* `next`不等於結束符並且不等於`*`, 則若`*p != '.' && *s != *p, return false`, 否則`return match(s + 1, p + 1)`,匹配下一個字符

* `next`等於`*`,則需要從`s`開始掃描，檢測是否存在匹配`*`後面的字符串. 比如`.*ef*`,需要匹配`ef`結尾的字符串, 但前面可以有任意多的字符,
	因此需要遍歷所有字符串直到匹配到`ef*`.即從`s`開始查找，`match(s + i + 1, p + 2)`

## Code
```c
bool isMatch(char *s,char *p)
{
	if (*p == '\0') // p為空
		return *s == '\0';
	if (*(p + 1) == '\0' || *(p + 1) != '*') {
		if (*s == '\0' || (*p != '.' && *s != *p))
			return false;
		return isMatch(s + 1, p + 1);
	}
	int i = -1, len = strlen(s);
	while (i < len && (i < 0 || *p == '.' || *(s + i) == *p)) {
		if (isMatch(s + i + 1, p + 2))
			return true;
		i++;
	}
	return false;
}
```
