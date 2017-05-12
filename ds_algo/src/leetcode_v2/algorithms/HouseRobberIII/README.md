## House Robber III

The thief has found himself a new place for his thievery again. There is only one entrance to this area, called the "root." Besides the root, each house has one and only one parent house. After a tour, the smart thief realized that "all houses in this place forms a binary tree". It will automatically contact the police if two directly-linked houses were broken into on the same night.

Determine the maximum amount of money the thief can rob tonight without alerting the police.

Example 1:

```
     3
    / \
   2   3
    \   \ 
     3   1
```
Maximum amount of money the thief can `rob = 3 + 3 + 1 = 7`.

Example 2:

```
     3
    / \
   4   5
  / \   \ 
 1   3   1
```
Maximum amount of money the thief can `rob = 4 + 5 = 9`.

## Solution

最开始想到直接层次遍历，获取每一层的值，比如Example 2为`[3,9,5]`,然后转化为[House Robber](../HouseRobber)求解。时间复杂度时O(n),空间复杂度是O(n)。

看提示说使用DFS，一个节点要么选择要么不选择，设当前节点为p，yes为选择p节点的值，no为不选择p节点的值,f(p)为p节点的值，则：

* 若p为null，则yes = 0， no = 0;
* 若选择p，则不能选择p的孩子节点，则`p->yes = f(p->left->no) + f(p->right->no) + p->val`;
* 若不选择p节点，则结果为孩子节点的最大值，即`p->no = max(f(p->left->yes), f(p->left->no)) + max(f(p->right->yes), f(p->right->no))`

```cpp
void dfs(TreeNode *root, int *yes, int *no) {
	if (root == nullptr) {
		*yes = 0;
		*no = 0;
		return;
	}
	int left_yes, left_no, right_yes, right_no;
	dfs(root->left, &left_yes, &left_no);
	dfs(root->right, &right_yes, &right_no);
	*yes = left_no + right_no + root->val;
	*no = max(left_yes, left_no) + max(right_yes, right_no);
}
```

最后返回`max(root->yes, root->no)`即可。

```cpp
int rob(TreeNode *root) {
	int yes = 0, no = 0;
	dfs(root, &yes, &no);
	return max(yes, no);
}
```

## 参考

1. [House Robber](../HouseRobber): 线性的情况
2. [House Robber II](../HouseRobberII)：线性的情况，首位相连
3. [House Robber III](../HouseRobberIII)：树形结构
