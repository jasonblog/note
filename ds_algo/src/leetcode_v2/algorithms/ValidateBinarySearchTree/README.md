## Validate Binary Search Tree

Given a binary tree, determine if it is a valid binary search tree (BST).

Assume a BST is defined as follows:

* The left subtree of a node contains only nodes with keys less than the node's key.
* The right subtree of a node contains only nodes with keys greater than the node's key.
* Both the left and right subtrees must also be binary search trees.

## Solution

根據定義，很容易寫出遞歸代碼:
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
即若當前節點小於等於左孩子或者大於等於右孩子，則一定不是BST。


否則，遞歸判斷左子樹和右子樹都必須是BST

```
      10
     / \
    5  15
      /  \
     6   20
```

顯然根據以上判斷，則上面的是一棵BST，但事實上，顯然這不是BST。

這是因為不僅要求左子樹和右子樹必須是BST，還要求左子樹的所有節點都必須小於根節點，並且右子樹的所有節點都必須大於根節點

根據BST特性，中序遍歷序列是有序的，因此我們可以通過中序遍歷的方式判斷，把序列壓入vector中,最後判斷vector是否有序即可.

不過其實我們並不需要保存到vector中，我們只需要和前一個遍歷的節點比較即可，若當前節點的值比前一個值小，則不是BST

我們可以使用prev保存前一個遍歷的值，初始化為`INT_MIN`, 
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
但是以下這棵樹:
```

		INT_MIN
		/    \
               NULL  NULL
```
即根節點是最小值，沒有孩子節點，顯然一個節點是BST，但根據以上代碼判斷會出錯.

當然只要`INT_MIN`是第一個訪問節點，就不能直接和prev判斷，否則出錯,比如
```
		INT_MAX
		/    \
	     INT_MIN NULL
```
由於判斷左子樹不是BST，而導致整個樹不是BST。

因此必須判斷是否第一個訪問節點處理
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

*總結:* 這個題目不是很難，當陷阱很多，尤其是比較前一個數的時候，必須判斷是否第一個訪問節點
