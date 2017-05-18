## Anagrams 

Given an array of strings, return all groups of strings that are anagrams.

Note: All inputs will be in lower-case.

## Solution

两个单词的字符相同，只是排列顺序不同，称为回文构词法(anagrams), 比如tea eta ate aet都是anagrams。

题目要求找出所有的anagrams组，比如`["dog","cat","god","tac", "hello", "world"]`, 应该返回`["dog","cat","god","tac"]`,
因为dog 和 god是一组， cat和tac是一组，即

```
[
  ["dog", "god"],
  ["cat", "tac"]
]
```

判断两个字符串是不是anagrams，最简单的方法是对字符排序然后判断是否相等。比如`tac`、`cat`按字符排序后分别为`act`、`act`，显然是相等的，因此互为anagrams.

给定一个字符串数组，如何找出所有的anagrams组。当一个字符串处理时，我们需要判断之前是否出现过互为anagrams的字符串。
如果没有出现过，我们保存到一个map中，key保存字符串的排序后的字符串，value保存第一次出现的索引。若已经出现过，并且当前是
第二次出现，则前一次出现的和当前的字符串互为anagrams

```cpp
vector<string> anagrams(const vector<string>& strs) {
	unordered_map<string, int> m;
	vector<string> result;
	for (int i = 0; i < strs.size(); ++i) {
		string s = strs[i];
		sort(begin(s), end(s));
		if (m.find(s) == m.end()) {
			m[s] = i;
		} else {
			if (m[s] >= 0) {
				result.push_back(strs[m[s]]);
				m[s] = -1;
			}
			result.push_back(strs[i]);
		}
	}
	return result;
}
```
