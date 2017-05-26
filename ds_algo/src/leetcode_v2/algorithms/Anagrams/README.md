## Anagrams 

Given an array of strings, return all groups of strings that are anagrams.

Note: All inputs will be in lower-case.

## Solution

兩個單詞的字符相同，只是排列順序不同，稱為迴文構詞法(anagrams), 比如tea eta ate aet都是anagrams。

題目要求找出所有的anagrams組，比如`["dog","cat","god","tac", "hello", "world"]`, 應該返回`["dog","cat","god","tac"]`,
因為dog 和 god是一組， cat和tac是一組，即

```
[
  ["dog", "god"],
  ["cat", "tac"]
]
```

判斷兩個字符串是不是anagrams，最簡單的方法是對字符排序然後判斷是否相等。比如`tac`、`cat`按字符排序後分別為`act`、`act`，顯然是相等的，因此互為anagrams.

給定一個字符串數組，如何找出所有的anagrams組。當一個字符串處理時，我們需要判斷之前是否出現過互為anagrams的字符串。
如果沒有出現過，我們保存到一個map中，key保存字符串的排序後的字符串，value保存第一次出現的索引。若已經出現過，並且當前是
第二次出現，則前一次出現的和當前的字符串互為anagrams

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
