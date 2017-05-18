## Binary Tree Inorder Traversal

Given a binary tree, return the inorder traversal of its nodes' values.

For example:
Given binary tree `{1,#,2,3}`,
```
   1
    \
     2
    /
   3
```
return `[1,3,2]`.

Note: Recursive solution is trivial, could you do it iteratively?

OJ's Binary Tree Serialization:
The serialization of a binary tree follows a level order traversal, where '#' signifies a path terminator where no node exists below.

Here's an example:
```
   1
  / \
 2   3
    /
   4
    \
     5
```
The above binary tree is serialized as "`{1,2,3,#,#,4,#,#,5}`".

## Solution

递归方法
```cpp
void recursive(vector<int> &result, TreeNode *root) {
	if (root == nullptr)
		return;
	recursive(result, root->left);
	result.push_back(root->val);
	recursive(result, root->right);
}
```

迭代方法

```cpp
void loop(vector<int> &result, TreeNode *root) {
	if (root == nullptr)
		return;
	stack<TreeNode *> s;
	TreeNode *cur = root;
	while (cur || !s.empty()) {
		while (cur) {
			s.push(cur);
			cur = cur->left;
		}
		cur = s.top();
		s.pop();
		result.push_back(cur->val);
		cur = cur->right;
	}
}
```
