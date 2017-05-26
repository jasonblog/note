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

這題和[Path Sum I](../PathSum)一樣使用前序遍歷的方法，不過遍歷的同時需要保存當前的值。

設當前訪問的節點為p，已訪問的節點保存於cur, 結果集result, 目標值sum

* 若p是空節點，直接返回
* p值壓入cur，`sum -= p->val`
* 若p是葉子節點，並且`sum == 0`, 把cur壓入result結果集中
* 否則p不是葉子節點，遞歸探測左孩子、右孩子.

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
