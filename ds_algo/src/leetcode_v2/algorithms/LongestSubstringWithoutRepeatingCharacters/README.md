# Longest Substring Without Repeating Characters
Given a string, find the length of the longest substring without repeating characters.
For example, the longest substring without repeating letters for "abcabcbb" is "abc", which the length is 3.
For "bbbbb" the longest substring is "b", with the length of 1.
# Solution

## Solution 1
一次扫描该字符串，并把扫描到的字符放入vector中,当遇到字符在容器中，则移除该字符之前的所有字符。
比如abcabcbb,扫描到abca时，前缀容器值为abc，当前字符a，已经存在前缀容器中，需要先移除a之前的所有字符，再压入前缀容器，此时前缀容器为bca。
时间复杂度为O(n),因为虽然每次需要在vector中查找是否存在该字符，但这个vector显然至多256即acii码，和字符串长度没有线性关系，空间复杂度为O（1）（最大空间256）。
代码：
```c
int lengthOfLongestSubstring(string s) {
	vector<char> v;
	unsigned int max = 0;
	for (size_t i = 0; i < s.size(); ++i) {
		auto pos = find(v.begin(), v.end(), s[i]);
		if (pos != v.end()) {
			max = max >= v.size() ? max : v.size();
			v.erase(v.begin(), pos + 1);
		}
			v.push_back(s[i]);
	}
	return max >= v.size() ? max : v.size();
    }
```

## Solution 2

当一个最长子串结束时（即遇到重复的字符），新的子串的长度是与第一个重复的字符的下标有关的，如果该下标在上一个最长子串起始位置之前，则dp[i] = dp[i-1] + 1，
即上一个最长子串的起始位置也是当前最长子串的起始位置；如果该下标在上一个最长子串起始位置之后，则新的子串是从该下标之后开始的
```c
int lengthOfLongestSubstring(char *s) {
	int hash[256],len = strlen(s), i;
	int result = 0, curLen = 0, pos = 0,maxPos;
	if (len < 2)
		return len;
	memset(hash, -1, sizeof(hash));
	for (i = 0; i < len; ++i) {
		if (hash[s[i]] == -1) {
			curLen++;
			hash[s[i]] = i;
		} else {
			result = max(result, curLen); // update result
			maxPos = max(pos, hash[s[i]]);
			curLen = i - maxPos;
			pos = maxPos;
			hash[s[i]] = i;
		}
	}
	return result >= curLen ? result : curLen;
}
```

时间复杂度O(n),空间复杂度O(1)
