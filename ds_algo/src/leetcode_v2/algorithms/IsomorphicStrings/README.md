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

题目的意思就是找一个字母的一一映射，通过映射能够把其中一个单词变化成另一个单词，比如`paper title`, `p->t, a->i, e->l,r->e`, 映射关系不能相交,即
不能出现一对多和多对一的关系,比如不能出现`a->b, a->c` 或者 `b->a, c->a`.

我们可以使用map记录之前的映射关系，

* 若s[i], t[i], 若map[s[i]] != t[i]，则返回false
* 若存在map[k] = t[i], 返回false， 因为此时说明已经有关系映射成t[i]
* 否则加入map，即map[s[i]] = t[i]

使用Hashmap能够O(1)的判断key是否存在，但需要判断value是否存在需要O(n),因此可以使用两个map来减少时间复杂度。

我们使用两个map，如果f 和 g是映射关系，则让赋予他们一样的值，即map1[f] = value, map2[g] = value

则:

* 若`map1[s[i]] != map2[t[i]]`, 返回`false`
* 否则`map1[s[i]] = map2[t[i]] = newValue`

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
