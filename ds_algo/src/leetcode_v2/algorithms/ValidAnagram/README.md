## Valid Anagram

Given two strings s and t, write a function to determine if t is an anagram of s.

For example,
`s = "anagram"`, `t = "nagaram"`, return `true`.
`s = "rat"`, `t = "car"`, return `false`.

Note:
*You may assume the string contains only lowercase alphabets.*

## Solution

* 如果两个字符串长度不一样，返回`false`。
* 两个字符串所有出现字符以及数量相同，返回`true`。使用hash表首先存储第一个字符串的字符个数，然后遍历第二个字符串，每出现一个字符，对应hash表计数-1，若出现负数，则返回`false`。

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
