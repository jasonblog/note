## Implement Trie (Prefix Tree)

Implement a trie with insert, search, and startsWith methods.

Note:
You may assume that all inputs are consist of lowercase letters a-z.

## Solution

[java 实现版本](https://github.com/krystism/algorithms/tree/master/trie)

```cpp
class TrieNode {
	public:
		static const int SPACE_SIZE = 26;
		TrieNode *children[SPACE_SIZE];
		bool exist;

		TrieNode() {
			for (int i = 0; i < SPACE_SIZE; ++i) {
				children[i] = nullptr;
			}
			exist = false;
		}
};
class Trie {
	public:
		Trie() {
			root = new TrieNode();
		}
		void insert(string s) {
			TrieNode *p = root;
			for (char c : s) {
				int index = c - 'a';
				if (p->children[index] == nullptr) {
					p->children[index] = new TrieNode();
				}
				p = p->children[index];
			}
			p->exist = true;
		}
		bool search(const string key) const {
			TrieNode *p = root;
			for (char c : key) {
				int index = c - 'a';
				if (p->children[index] == nullptr)
					return false;
				p = p->children[index];
			}
			return p->exist;
		}
		bool startsWith(const string prefix) const {
			TrieNode *p = root;
			for (char c : prefix) {
				int index = c - 'a';
				if (p->children[index] == nullptr)
					return false;
				p = p->children[index];
			}
			return true;
		}
	private:
		TrieNode *root;
};
```
## 扩展

[Add and Search Word - Data structure design](../AddandSearchWord): 实现含有任意字符的搜索
