## Path Sum II

Given a binary tree and a sum, find all root-to-leaf paths where each path's sum equals the given sum.

For example:
Given the below binary tree and `sum = 22`,
```
              5
             / \
            4   8
           /   / \
          11  13  4
         /  \    / \
        7    2  5   1
```
return
```
[
   [5,4,11,2],
   [5,8,4,5]
]
```

这题和[Path Sum I](../PathSum)一样使用前序遍历的方法，不过遍历的同时需要保存当前的值。

设当前访问的节点为p，已访问的节点保存于cur, 结果集result, 目标值sum

* 若p是空节点，直接返回
* p值压入cur，`sum -= p->val`
* 若p是叶子节点，并且`sum == 0`, 把cur压入result结果集中
* 否则p不是叶子节点，递归探测左孩子、右孩子.

## Code
```cpp
void pathSum(vector<vector<int>> &result, vector<int> cur, TreeNode *root, int sum) {
    if (root == NULL)
	    return;
    cur.push_back(root->val);
    sum -= root->val;
    if (isLeft(root) && sum == 0) {
	    result.push_back(cur);
	    return;
    }
    pathSum(result, cur, root->left, sum);
    pathSum(result, cur, root->right, sum);
}
```
