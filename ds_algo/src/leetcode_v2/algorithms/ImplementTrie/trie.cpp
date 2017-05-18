#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <iostream>
using namespace std;
class TrieNode
{
public:
    static const int SPACE_SIZE = 26;
    TrieNode* children[SPACE_SIZE];
    bool exist;

    TrieNode()
    {
        for (int i = 0; i < SPACE_SIZE; ++i) {
            children[i] = nullptr;
        }

        exist = false;
    }
};
class Trie
{
public:
    Trie()
    {
        root = new TrieNode();
    }
    void insert(const string s)
    {
        TrieNode* p = root;

        for (char c : s) {
            int index = c - 'a';

            if (p->children[index] == nullptr) {
                p->children[index] = new TrieNode();
            }

            p = p->children[index];
        }

        p->exist = true;
    }
    bool search(const string key) const
    {
        TrieNode* p = root;

        for (char c : key) {
            int index = c - 'a';

            if (p->children[index] == nullptr) {
                return false;
            }

            p = p->children[index];
        }

        return p->exist;
    }
    bool startsWith(const string prefix) const
    {
        TrieNode* p = root;

        for (char c : prefix) {
            int index = c - 'a';

            if (p->children[index] == nullptr) {
                return false;
            }

            p = p->children[index];
        }

        return true;
    }
private:
    TrieNode* root;
};
int main(int argc, char** argv)
{
    Trie trie;
    trie.insert("");
    trie.insert("abcd");
    cout << trie.search("abc") << endl;
    cout << trie.search("abcd") << endl;;
    cout << trie.startsWith("abc") << endl;
    cout << trie.startsWith("abcd") << endl;
    cout << trie.search("") << endl;
    cout << trie.startsWith("") << endl;
    return 0;
}
