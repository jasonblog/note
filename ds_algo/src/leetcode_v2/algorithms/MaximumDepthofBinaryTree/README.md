## Maximum Depth of Binary Tree

Given a binary tree, find its maximum depth.

The maximum depth is the number of nodes along the longest path from the root node down to the farthest leaf node.

## Solution

遞歸，樹的高度等於左子樹和右子樹深度大的

## Code
```c
int maxDepth(struct TreeNode *root)
{
	if (root == NULL)
		return 0;
	return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}
```

## 擴展

求樹的最小深度[Minimum Depth of Binary Tree](../MinimumDepthofBinaryTree)
