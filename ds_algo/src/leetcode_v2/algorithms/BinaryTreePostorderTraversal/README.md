## Binary Tree Postorder Traversal

Given a binary tree, return the postorder traversal of its nodes' values.

For example:
Given binary tree {1,#,2,3},
```
   1
    \
     2
    /
   3
```
return [3,2,1].

## Solution 1

遞歸法, 這個方法沒有什麼難度

## Solution 2

前序遍歷，然後把結果reverse

## Solution 3

根據後序遍歷的方式，最先訪問的是左孩子節點的最左的最後一個節點,比如

```

              1
             / \
            2   3
           / \
          5   4
	   \   \
            6  7
```
顯然最先訪問的是6，因此需要把之前的節點壓入棧。
```cpp
void pushLeftOrRight(TreeNode *p, stack<TreeNode*> &s) {
	while (p) {
		s.push(p);
		p = p->left != nullptr ? p->left : p->right;
	}
}
```

顯然先是從root開始，壓入的順序依次為`1 2 5 6`

然後依次出棧就是訪問順序，當出棧前需要判斷是否還有右孩子沒有訪問

比如當前已訪問2(2還沒有出棧)，應該開始訪問右孩子, 即調用`pushLeftOrRight(p->right, s)`

那怎麼知道有沒有訪問右孩子呢？我們發現我們有兩次機會會到達2，一次是訪問完左孩子，一次是訪問完右孩子，即訪問完5和4

我們可以發現，只要2出棧前，看之前訪問的是否是左孩子即可，若是左孩子，說明右孩子沒有訪問
```cpp
vector<int> postorderTraversal(TreeNode *root) {
	vector<int> result;
	if (root == nullptr)
		return result;
	stack<TreeNode*>s;
	pushLeftOrRight(root, s);
	while (!s.empty()) {
		TreeNode *p = getAndPop(s); // s.top(), s.pop()
		result.push_back(p->val);
		TreeNode *next = s.empty() ? nullptr : s.top();
		if (next && next->left == p)
			pushLeftOrRight(next->right, s);
	}
	return result;
}
```
