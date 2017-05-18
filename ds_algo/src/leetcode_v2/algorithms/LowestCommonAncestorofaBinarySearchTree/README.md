## Lowest Common Ancestor of a Binary Search Tree 

Given a binary search tree (BST), find the lowest common ancestor (LCA) of two given nodes in the BST.

According to the [definition of LCA on Wikipedia](https://en.wikipedia.org/wiki/Lowest_common_ancestor): “The lowest common ancestor is defined between two nodes v and w as the lowest node in T that has both v and w as descendants (where we allow a node to be a descendant of itself).”

```
        _______6______
       /              \
    ___2__          ___8__
   /      \        /      \
   0      _4       7       9
         /  \
         3   5
```

For example, the lowest common ancestor (LCA) of nodes 2 and 8 is 6. Another example is LCA of nodes 2 and 4 is 2, since a node can be a descendant of itself according to the LCA definition.

## Solution

由于是BST，若p,q节点都小于root，则一定在左子树，若p，q都大于root，则一定在右子树,否则一个大于root,一个小于root，则root就是最低祖先节点

```cpp
class Solution {
public:
	TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q) {
		if (root == nullptr)
			return nullptr;
		return lowestCommonAncestor(root, p->val, q->val);
	}
private:
	TreeNode *lowestCommonAncestor(TreeNode *root, int v1, int v2) {
		if (root->val > v1 && root->val > v2)
			return lowestCommonAncestor(root->left, v1, v2);
		if (root->val < v1 && root->val < v2)
			return lowestCommonAncestor(root->right, v1, v2);
		return root;
	}
};
```

## 相关题目

* [Lowest Common Ancestor of a Binary Search Tree](../LowestCommonAncestorofaBinarySearchTree):求BST最低公共祖先节点
* [Lowest Common Ancestor of a Binary Tree](../LowestCommonAncestorofaBinaryTree):求二叉树最低公共祖先节点
