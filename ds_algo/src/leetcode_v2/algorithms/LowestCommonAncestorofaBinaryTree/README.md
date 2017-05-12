## Lowest Common Ancestor of a Binary Tree

Given a binary tree, find the lowest common ancestor (LCA) of two given nodes in the tree.

According to the definition of LCA on Wikipedia: “The lowest common ancestor is defined between two nodes v and w as the lowest node in T that has both v and w as descendants (where we allow a node to be a descendant of itself).”

```
        _______3______
       /              \
    ___5__          ___1__
   /      \        /      \
   6      _2       0       8
         /  \
         7   4
```

For example, the lowest common ancestor (LCA) of nodes 5 and 1 is 3. Another example is LCA of nodes 5 and 4 is 5, since a node can be a descendant of itself according to the LCA definition.

## Solution

判断两个节点:

* 若两个节点分别在左右子树，则当前节点就是最低公共祖先节点
* 若两个节点都在左子树，则最低公共祖先节点在左子树
* 若两个节点都在右子树，则最低公共祖先节点在右子树
简单的方法是:
```cpp
TreeNode *LCA(TreeNode *root, TreeNode *p, TreeNode *q)
{
	bool left = findAny(root->left, p, q);
	bool right = findAny(root->right, p, q);
	if (left && right)
		return root;
	if (left)
		return LCA(root->left, p, q);
	if (right)
		return LCA(root->right, p, q);
	return nullptr;
}
```

但以上方法复杂度在于findAny，即查找树中是否存在两个节点中的任意一个节点,复杂度为O(n),总复杂度为O(n<sup>2</sup>)

我们可以自底向上遍历结点，一旦遇到结点等于p或者q，则将其向上传递给它的父结点。父结点会判断它的左右子树是否都包含其中一个结点，如果是，则父结点一定是这两个节点p和q的LCA，传递父结点到root。如果不是，我们向上传递其中的包含结点p或者q的子结点，或者NULL(如果子结点不包含任何一个)。该方法时间复杂度为O(n)。

```cpp
TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
{
	if (root == nullptr)
		return nullptr;
	if (root == p || root == q)
		return root;
	TreeNode *left = lowestCommonAncestor(root->left, p, q);
	TreeNode *right = lowestCommonAncestor(root->right, p, q);
	if (left && right)
		return root;
	return left ? left:right;
}
```

也可以分别求出root到p节点路径和root到q节点的路径，然后求这两条路径的最后一个公共节点.

首先求root到p节点的路径:

```cpp
bool getPath(TreeNode *root, TreeNode *p, list<TreeNode *> &path) {
	if (root == nullptr)
		return p == nullptr;
	path.push_back(root);
	if (root == p)
		return true;
	if (getPath(root->left, p, path) || getPath(root->right, p, path))
		return true;
	else {
		path.pop_back();
		return false;
	}
}
```

然后求给定两条路径，求最后一个公共节点:

```cpp
TreeNode *findCommonNode(const list<TreeNode *> &l1, const list<TreeNode *> &l2) {
	int n = l1.size();
	int m = l2.size();
	auto i = begin(l1), j = begin(l2);
	TreeNode *last = nullptr;
	while (i != end(l1) && j != end(l2)) {
		if (*i == *j) {
			last = *i;
		}
		i++, j++;
	}
	return last;
}
```

于是LCA转化为:

```cpp
TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
{
	if (root == nullptr)
		return nullptr;
	list<TreeNode *> l1, l2;
	getPath(root, p, l1);
	getPath(root, q, l2);
	/*
	for_each(begin(l1), end(l1), [](TreeNode *t){cout << t->val << ' ';});
	cout << endl;
	for_each(begin(l2), end(l2), [](TreeNode *t){cout << t->val << ' ';});
	cout << endl;
	*/
	return findCommonNode(l1, l2);
}
```

## 相关题目

* [Lowest Common Ancestor of a Binary Search Tree](../LowestCommonAncestorofaBinarySearchTree):求BST最低公共祖先节点
* [Lowest Common Ancestor of a Binary Tree](../LowestCommonAncestorofaBinaryTree):求二叉树最低公共祖先节点
