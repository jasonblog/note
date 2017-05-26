## Add and Search Word - Data structure design

Design a data structure that supports the following two operations:

```
void addWord(word)
bool search(word)
```

`search(word)` can search a literal word or a regular expression string containing only letters a-z or `.`. 

A `.` means it can represent any one letter.

For example:

```
addWord("bad")
addWord("dad")
addWord("mad")
search("pad") -> false
search("bad") -> true
search(".ad") -> true
search("b..") -> true
```

Note:
You may assume that all words are consist of lowercase letters `a-z`.

click to show hint.

You should be familiar with how a Trie works. If not, please work on this problem: [Implement Trie (Prefix Tree)](../ImplementTrie) first.

## Solution

Trie樹的擴展，如果搜索字符，和Trie樹一樣，若為".", 則DFS之

```cpp
bool search(TrieNode *p, const char *target) const {
			if (p == nullptr)
				return false;
			int len = strlen(target);
			if (target == nullptr || len == 0) { // 到底單詞尾部
				return p->exist;
			}
			char c = *target;
			if (c != '.') { // 字符不是"."
				int index = c - 'a';
				return search(p->children[index], target + 1);
			} else { // 字符是".", DFS
				for (int i = 0; i < TrieNode::SPACE_SIZE; ++i) {
					if (search(p->children[i], target + 1))
						return true;
				}
				return false;
			}
		}
```

搜索單詞`word`, 只需要調用`search(root, word.c_str())`即可.

## 擴展

[Implement Trie (Prefix Tree)](../ImplementTrie): Trie樹實現
