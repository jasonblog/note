## Path Sum

Given a binary tree and a sum, determine if the tree has a root-to-leaf path such that adding up all the values along the path equals the given sum.

For example:
Given the below binary tree and `sum = 22`,
```
              5
             / \
            4   8
           /   / \
          11  13  4
         /  \      \
        7    2      1
```
return true, as there exist a root-to-leaf path `5->4->11->2` which sum is 22.

## Solution

遞歸題

* 如果p是葉子節點，則若`p的值等於sum`，`return true`， 否則`return false`
* 若p不是葉子節點，則一定存在孩子節點，左孩子或者右孩子滿足其中一個和為`sum - p->val`即可,即
`return hasPath(p->left, sum - p->val) || hasPath(p->right, sum - p->right)`

## Code
```c
bool hasPathSum(struct TreeNode *root, int sum) {
	if (root == NULL)
		return false;
	if (root->left == NULL && root->right == NULL)
		return root->val == sum;
	if (!root->left || !hasPathSum(root->left, sum - root->val)) {
		return hasPathSum(root->right, sum - root->val);
	}
	return true;
}
```

## 擴展

見[Path Sum II](../PathSum2).
