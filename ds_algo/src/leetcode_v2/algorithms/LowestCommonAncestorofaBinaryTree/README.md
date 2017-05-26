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

判斷兩個節點:

* 若兩個節點分別在左右子樹，則當前節點就是最低公共祖先節點
* 若兩個節點都在左子樹，則最低公共祖先節點在左子樹
* 若兩個節點都在右子樹，則最低公共祖先節點在右子樹
簡單的方法是:
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

但以上方法複雜度在於findAny，即查找樹中是否存在兩個節點中的任意一個節點,複雜度為O(n),總複雜度為O(n<sup>2</sup>)

我們可以自底向上遍歷結點，一旦遇到結點等於p或者q，則將其向上傳遞給它的父結點。父結點會判斷它的左右子樹是否都包含其中一個結點，如果是，則父結點一定是這兩個節點p和q的LCA，傳遞父結點到root。如果不是，我們向上傳遞其中的包含結點p或者q的子結點，或者NULL(如果子結點不包含任何一個)。該方法時間複雜度為O(n)。

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

也可以分別求出root到p節點路徑和root到q節點的路徑，然後求這兩條路徑的最後一個公共節點.

首先求root到p節點的路徑:

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

然後求給定兩條路徑，求最後一個公共節點:

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

於是LCA轉化為:

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

## 相關題目

* [Lowest Common Ancestor of a Binary Search Tree](../LowestCommonAncestorofaBinarySearchTree):求BST最低公共祖先節點
* [Lowest Common Ancestor of a Binary Tree](../LowestCommonAncestorofaBinaryTree):求二叉樹最低公共祖先節點
