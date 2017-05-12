## Same Tree

Given two binary trees, write a function to check if they are equal or not.

Two binary trees are considered equal if they are structurally identical and the nodes have the same value. 

## Solution

递归

## Code
```c
bool isSameTree(struct TreeNode *p, struct TreeNode *q) {
	if (p == NULL)
		return q == NULL;
	if (q == NULL)
		return false;
	if (p->val != q->val)
		return false;
	return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
}
```
