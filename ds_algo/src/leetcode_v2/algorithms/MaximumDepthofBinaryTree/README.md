## Maximum Depth of Binary Tree

Given a binary tree, find its maximum depth.

The maximum depth is the number of nodes along the longest path from the root node down to the farthest leaf node.

## Solution

递归，树的高度等于左子树和右子树深度大的

## Code
```c
int maxDepth(struct TreeNode *root)
{
	if (root == NULL)
		return 0;
	return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}
```

## 扩展

求树的最小深度[Minimum Depth of Binary Tree](../MinimumDepthofBinaryTree)
