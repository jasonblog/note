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

递归题

* 如果p是叶子节点，则若`p的值等于sum`，`return true`， 否则`return false`
* 若p不是叶子节点，则一定存在孩子节点，左孩子或者右孩子满足其中一个和为`sum - p->val`即可,即
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

## 扩展

见[Path Sum II](../PathSum2).
