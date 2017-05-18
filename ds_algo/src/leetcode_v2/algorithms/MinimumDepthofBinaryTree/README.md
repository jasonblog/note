## Minimum Depth of Binary Tree

Given a binary tree, find its minimum depth.

The minimum depth is the number of nodes along the shortest path from the root node down to the nearest leaf node.

## Solution

题目不难，但犯了好几个错误:

* 最小深度的概念没有弄清楚，必须是到叶子节点中最短的路径
```
  1
 /
3
```
以上述的最小深度不是1而是2

* 利用宏求最小值，重复计算导致超时
```
#define MIN(a, b) ((a) < (b) : (a) : (b))

MIN(minDepth(root->left), minDepth(root->right));

展开为

minDepth(root->left) < minDepth(root->right) ? minDepth(root->left) : minDepth(root->right);
```

发现每次计算左右子树都是两倍计算量

** 因此以后尽量不要使用宏定义， 而尽量使用内联函数 **

# Code
```cpp
int minDepth(struct TreeNode *root) {
	if (root == NULL)
		return 0;
	if (root->left && root->right)
		return min(minDepth(root->left), minDepth(root->right)) + 1; 
	else if (root->left)
		return minDepth(root->left) + 1;
	else
		return minDepth(root->right) + 1;
}
```

## 扩展

最大深度[Maximum Depth of Binary Tree](../MaximumDepthofBinaryTree)
