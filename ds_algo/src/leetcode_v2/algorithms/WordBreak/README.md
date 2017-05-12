## Word Break

Given a string s and a dictionary of words dict, determine if s can be segmented into a space-separated sequence of one or more dictionary words.

For example, given
s = `"leetcode"`,
dict = `["leet", "code"]`.

Return true because `"leetcode"` can be segmented as `"leet code"`.

## Solution

类似DP思想:

使用dp[i]表示前面i个字符(0 ~ i - 1)是否可以break, 显然dp[i]取决于i之前是否可以break，并且`dp[0] = true`,即

``` 
for j from 0 to 1
	if dp[j] && s[i:j] in dict
		dp[i] = true
		break
```

于是实现代码为:

```cpp
bool wordBreak(string s, const unordered_set<string> &dict) {
	int n = s.size();
	vector<bool> canBreak(n + 1, false);
	canBreak[0] = true;
	for (int i = 1; i <= n; ++i) {
		for (int j = 0; j < i; ++j) {
			if (canBreak[j] && dict.find(s.substr(j, i - j)) != dict.end()) {
				canBreak[i] = true;
				break;
			}
		}
	}
	return canBreak[n];
}
```

## 扩展

[Word Break II](../WordBreakII): 打印所有的结果
