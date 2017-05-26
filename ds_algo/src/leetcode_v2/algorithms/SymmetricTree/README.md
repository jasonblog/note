## Symmetric Tree

Given a binary tree, check whether it is a mirror of itself (ie, symmetric around its center).

For example, this binary tree is symmetric:
```
    1
   / \
  2   2
 / \ / \
3  4 4  3
```
But the following is not:
```
    1
   / \
  2   2
   \   \
   3    3
```
Note:

* Bonus points if you could solve it both recursively and iteratively.

* confused what `"{1,#,2,3}"` means? > read more on how binary tree is serialized on OJ.

## Solution

對稱樹，即根據根節點劃分的兩個子樹是軸對稱的。左子樹和右子樹分別比較即可:

* 左節點和右節點的值相等。
* 左節點的左孩子和右節點的右孩子對稱，並且左節點的右孩子和右節點的左孩子對稱

滿足以上兩個條件即是對稱樹

## Code
```c
bool isLeftRightSymmetric(struct TreeNode *left, struct TreeNode *right)
{
	if (left == NULL)
		return right == NULL;
	if (right == NULL)
		return false;
	return left->val == right->val
		&& isLeftRightSymmetric(left->left, right->right)
		&& isLeftRightSymmetric(left->right,right->left);
}
bool isSymmetric(struct TreeNode *root) {
	if (root == NULL)
		return true;
	return isLeftRightSymmetric(root->left, root->right);
}
```
