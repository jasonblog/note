## Minimum Depth of Binary Tree

Given a binary tree, find its minimum depth.

The minimum depth is the number of nodes along the shortest path from the root node down to the nearest leaf node.

## Solution

題目不難，但犯了好幾個錯誤:

* 最小深度的概念沒有弄清楚，必須是到葉子節點中最短的路徑
```
  1
 /
3
```
以上述的最小深度不是1而是2

* 利用宏求最小值，重複計算導致超時
```
#define MIN(a, b) ((a) < (b) : (a) : (b))

MIN(minDepth(root->left), minDepth(root->right));

展開為

minDepth(root->left) < minDepth(root->right) ? minDepth(root->left) : minDepth(root->right);
```

發現每次計算左右子樹都是兩倍計算量

** 因此以後儘量不要使用宏定義， 而儘量使用內聯函數 **

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

## 擴展

最大深度[Maximum Depth of Binary Tree](../MaximumDepthofBinaryTree)
