## Isomorphic Strings 

Given two strings s and t, determine if they are isomorphic.

Two strings are isomorphic if the characters in s can be replaced to get t.

All occurrences of a character must be replaced with another character while preserving the order of characters. No two characters may map to the same character but a character may map to itself.

For example,
```
Given "egg", "add", return true.

Given "foo", "bar", return false.

Given "paper", "title", return true.
```

Note:
You may assume both s and t have the same length.

## Solution

題目的意思就是找一個字母的一一映射，通過映射能夠把其中一個單詞變化成另一個單詞，比如`paper title`, `p->t, a->i, e->l,r->e`, 映射關係不能相交,即
不能出現一對多和多對一的關係,比如不能出現`a->b, a->c` 或者 `b->a, c->a`.

我們可以使用map記錄之前的映射關係，

* 若s[i], t[i], 若map[s[i]] != t[i]，則返回false
* 若存在map[k] = t[i], 返回false， 因為此時說明已經有關係映射成t[i]
* 否則加入map，即map[s[i]] = t[i]

使用Hashmap能夠O(1)的判斷key是否存在，但需要判斷value是否存在需要O(n),因此可以使用兩個map來減少時間複雜度。

我們使用兩個map，如果f 和 g是映射關係，則讓賦予他們一樣的值，即map1[f] = value, map2[g] = value

則:

* 若`map1[s[i]] != map2[t[i]]`, 返回`false`
* 否則`map1[s[i]] = map2[t[i]] = newValue`

```cpp
bool isIsomorphic(string s, string t)
{
    int map1[N] = {0}, map2[N] = {0};
    int n = s.size();
    if (n != t.size())
	    return false;
    for (int i = 0; i < n; ++i) {
	    char a = s[i], b = t[i];
	    if (map1[a] != map2[b])
		    return false;
	    if (map1[a] == 0) {
		    map1[a]  = map2[b] = i + 1; // avoid 0
	    }
    }
    return true;
}
```
