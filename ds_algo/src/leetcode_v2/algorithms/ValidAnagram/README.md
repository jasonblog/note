## Valid Anagram

Given two strings s and t, write a function to determine if t is an anagram of s.

For example,
`s = "anagram"`, `t = "nagaram"`, return `true`.
`s = "rat"`, `t = "car"`, return `false`.

Note:
*You may assume the string contains only lowercase alphabets.*

## Solution

* 如果兩個字符串長度不一樣，返回`false`。
* 兩個字符串所有出現字符以及數量相同，返回`true`。使用hash表首先存儲第一個字符串的字符個數，然後遍歷第二個字符串，每出現一個字符，對應hash表計數-1，若出現負數，則返回`false`。

```c
bool isAnagram(char *s, char *t)
{
	int len1 = strlen(s), len2 = strlen(t);
	if (len1 != len2)
		return false;
	int table[26] = {0};
	for (int i = 0; i < len1; ++i)
		table[s[i] - 'a']++;
	for (int i = 0; i < len2; ++i) {
		if (table[t[i] - 'a'] <= 0)
			return false;
		table[t[i] - 'a']--;
	}
	return true;
}
```
