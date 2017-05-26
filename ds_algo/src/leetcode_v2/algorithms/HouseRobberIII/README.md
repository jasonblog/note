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

最開始想到直接層次遍歷，獲取每一層的值，比如Example 2為`[3,9,5]`,然後轉化為[House Robber](../HouseRobber)求解。時間複雜度時O(n),空間複雜度是O(n)。

看提示說使用DFS，一個節點要麼選擇要麼不選擇，設當前節點為p，yes為選擇p節點的值，no為不選擇p節點的值,f(p)為p節點的值，則：

* 若p為null，則yes = 0， no = 0;
* 若選擇p，則不能選擇p的孩子節點，則`p->yes = f(p->left->no) + f(p->right->no) + p->val`;
* 若不選擇p節點，則結果為孩子節點的最大值，即`p->no = max(f(p->left->yes), f(p->left->no)) + max(f(p->right->yes), f(p->right->no))`

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

最後返回`max(root->yes, root->no)`即可。

```cpp
int rob(TreeNode *root) {
	int yes = 0, no = 0;
	dfs(root, &yes, &no);
	return max(yes, no);
}
```

## 參考

1. [House Robber](../HouseRobber): 線性的情況
2. [House Robber II](../HouseRobberII)：線性的情況，首位相連
3. [House Robber III](../HouseRobberIII)：樹形結構
