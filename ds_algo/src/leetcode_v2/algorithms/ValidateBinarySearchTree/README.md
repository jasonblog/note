## Validate Binary Search Tree

Given a binary tree, determine if it is a valid binary search tree (BST).

Assume a BST is defined as follows:

* The left subtree of a node contains only nodes with keys less than the node's key.
* The right subtree of a node contains only nodes with keys greater than the node's key.
* Both the left and right subtrees must also be binary search trees.

## Solution

根据定义，很容易写出递归代码:
```cpp
bool isValidBST(TreeNode *root)
{
	if (root == NULL)
		return true;
	if (root->left && root->left->val >= root->val)
		return false;
	if (root->right && root->right->val <= root->val)
		return false;
	return isValidBST(root->left) && isValidBST(root->right);
}
```
即若当前节点小于等于左孩子或者大于等于右孩子，则一定不是BST。


否则，递归判断左子树和右子树都必须是BST

```
      10
     / \
    5  15
      /  \
     6   20
```

显然根据以上判断，则上面的是一棵BST，但事实上，显然这不是BST。

这是因为不仅要求左子树和右子树必须是BST，还要求左子树的所有节点都必须小于根节点，并且右子树的所有节点都必须大于根节点

根据BST特性，中序遍历序列是有序的，因此我们可以通过中序遍历的方式判断，把序列压入vector中,最后判断vector是否有序即可.

不过其实我们并不需要保存到vector中，我们只需要和前一个遍历的节点比较即可，若当前节点的值比前一个值小，则不是BST

我们可以使用prev保存前一个遍历的值，初始化为`INT_MIN`, 
```cpp
bool isValidBST(TreeNode *root) {
		if (root == NULL)
			return true;
		if (!isValidBST(root->left))
			return false;
		if (root->val <= prev)
			return false;
		prev = root->val;
		return isValidBST(root->right);
	}
```
但是以下这棵树:
```

		INT_MIN
		/    \
               NULL  NULL
```
即根节点是最小值，没有孩子节点，显然一个节点是BST，但根据以上代码判断会出错.

当然只要`INT_MIN`是第一个访问节点，就不能直接和prev判断，否则出错,比如
```
		INT_MAX
		/    \
	     INT_MIN NULL
```
由于判断左子树不是BST，而导致整个树不是BST。

因此必须判断是否第一个访问节点处理
```cpp
bool isValidBST(TreeNode *root) {
	int prev = INT_MIN;
	bool isFirst = true;
	return isValidBST(root, prev, isFirst);
}
bool isValidBST(TreeNode *root, int &prev, bool &isFirst) {
	if (root == nullptr)
		return true;
	if (!isValidBST(root->left, prev, isFirst))
		return false;
	if (isFirst) {
		isFirst = false;
	} else {
		if (root->val <= prev)
			return false;
	}
	prev = root->val;
	return isValidBST(root->right, prev, isFirst);
}
```

*总结:* 这个题目不是很难，当陷阱很多，尤其是比较前一个数的时候，必须判断是否第一个访问节点
