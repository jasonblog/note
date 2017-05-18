## Implement strStr()

Implement strStr().

Returns the index of the first occurrence of needle in haystack, or -1 if needle is not part of haystack.

Update (2014-11-02):
The signature of the function had been updated to return the index instead of the pointer. If you still see your function signature returns a char * or String, please click the reload button  to reset your code definition.

## Solution

模式匹配主要有三种方法:

1. 蛮力法，即BF算法
2. KMP算法
3. BM算法，工程应用比较多

具体算法见[由KMP算法谈到BM算法](http://blog.csdn.net/v_JULY_v/article/details/6545192)

本题使用BF算法TLE，只能使用KMP或者BM

```cpp
int kmp_strStr(string s, string p) {
	if (p.empty())
		return 0;
	if (s.empty()) {
		return -1;
	}
	int s_len = s.size(), p_len = p.size();
	int next[p_len], i = 0, j = -1;
	next[0] = -1;
	while (i < p_len - 1) {
		if (j == -1 || p[i] == p[j]) {
			i++, j++;
			if (p[i] != p[j])
				next[i] = j;
			else
				next[i] = next[j];
		} else {
			j = next[j];
		}
	}
	i = 0, j = 0;
	while (i < s_len && j < p_len) {
		if (j == -1 || s[i] == p[j])
			i++, j++;
		else
			j = next[j];
	}
	if (j == p_len)
		return i - j;
	return -1;
}
```

## 备注

java中的indexOf算法使用的是BF算法...
