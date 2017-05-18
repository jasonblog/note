## Balanced Binary Tree

Given a binary tree, determine if it is height-balanced.

For this problem, a height-balanced binary tree is defined as a binary tree in which the depth of the two subtrees of every node never differ by more than 1. 

## Solution

分别求左右子树的高度，如果高度之差大于1，则不是平衡树

而如果子树不是平衡树，结果也一定不是平衡树，因此再求左右子树的同时，顺便判断是否平衡树，能够提高速度

## Code
```cpp
bool isBalanced(TreeNode *root) {
	    bool unbalenced = false;
	    getHeight(unbalenced, root);
	    return !unbalenced;
}
int getHeight(bool &unbalenced, TreeNode *root) {
    if (root == NULL || unbalenced)
	    return -1;
    int left = getHeight(unbalenced, root->left);
    int right = getHeight(unbalenced, root->right);
    if (abs(left, right) > 1) {
	    unbalenced = true;
    }
    return 1 + (left > right ? left : right);
}
```
