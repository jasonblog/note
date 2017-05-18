## Longest Common Prefix

Write a function to find the longest common prefix string amongst an array of strings.

## Solution

逐一比较即可
```cpp
string longestCommonPrefix(vector<string>& strs) {
    int n = strs.size();
    if (n < 1)
	    return "";
    int m = strs[0].size();
    int i,j;
    for (i = 0; i < m; ++i) {
	    for (auto s : strs) {
		    if (i >= s.size() || s[i] != strs[0][i])
			    return strs[0].substr(0, i);
	    }
    }
    return strs[0];
}
```
