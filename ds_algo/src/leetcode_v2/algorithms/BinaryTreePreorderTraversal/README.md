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

前序遍历，使用递归，就太简单了。

如何使用迭代的方式,其实就是使用栈模拟递归(注意递归(DFS)和栈的关系，队列和BFS的关系)

先访问节点，然后压入右孩子，压入左孩子.
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
