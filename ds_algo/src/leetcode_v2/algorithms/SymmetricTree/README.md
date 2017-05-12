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

对称树，即根据根节点划分的两个子树是轴对称的。左子树和右子树分别比较即可:

* 左节点和右节点的值相等。
* 左节点的左孩子和右节点的右孩子对称，并且左节点的右孩子和右节点的左孩子对称

满足以上两个条件即是对称树

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
