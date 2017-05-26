# Longest Substring Without Repeating Characters
Given a string, find the length of the longest substring without repeating characters.
For example, the longest substring without repeating letters for "abcabcbb" is "abc", which the length is 3.
For "bbbbb" the longest substring is "b", with the length of 1.
# Solution

## Solution 1
一次掃描該字符串，並把掃描到的字符放入vector中,當遇到字符在容器中，則移除該字符之前的所有字符。
比如abcabcbb,掃描到abca時，前綴容器值為abc，當前字符a，已經存在前綴容器中，需要先移除a之前的所有字符，再壓入前綴容器，此時前綴容器為bca。
時間複雜度為O(n),因為雖然每次需要在vector中查找是否存在該字符，但這個vector顯然至多256即acii碼，和字符串長度沒有線性關係，空間複雜度為O（1）（最大空間256）。
代碼：
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

當一個最長子串結束時（即遇到重複的字符），新的子串的長度是與第一個重複的字符的下標有關的，如果該下標在上一個最長子串起始位置之前，則dp[i] = dp[i-1] + 1，
即上一個最長子串的起始位置也是當前最長子串的起始位置；如果該下標在上一個最長子串起始位置之後，則新的子串是從該下標之後開始的
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

時間複雜度O(n),空間複雜度O(1)
