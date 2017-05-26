## Binary Tree Preorder Traversal

Given a binary tree, return the preorder traversal of its nodes' values.

For example:
Given binary tree {1,#,2,3},
```
   1
    \
     2
    /
   3
```

return `[1,2,3]`.

Note: Recursive solution is trivial, could you do it iteratively?

## Solution

前序遍歷，使用遞歸，就太簡單了。

如何使用迭代的方式,其實就是使用棧模擬遞歸(注意遞歸(DFS)和棧的關係，隊列和BFS的關係)

先訪問節點，然後壓入右孩子，壓入左孩子.
```cpp
vector<int> preorderTraversal(TreeNode *root) {
	vector<int> result;
	if (root == NULL)
		return result;
	stack<TreeNode *>s;
	s.push(root);
	while (!s.empty()) {
		TreeNode *p = s.top();
		s.pop();
		result.push_back(p->val);
		if (p->right)
			s.push(p->right);
		if (p->left)
			s.push(p->left);
	}
	return result;
}
```
