## Binary Tree Level Order Traversal II

Given a binary tree, return the bottom-up level order traversal of its nodes' values. (ie, from left to right, level by level from leaf to root).

For example:
Given binary tree `{3,9,20,#,#,15,7}`,
```
    3
   / \
  9  20
    /  \
   15   7
```
return its bottom-up level order traversal as:
```
[
  [15,7],
  [9,20],
  [3]
]
```
confused what "`{1,#,2,3}`" means? > read more on how binary tree is serialized on OJ.

## Solution

思路就是常规的BFS，然后把结果reverse ?

应该还有其他更优算法？
```cpp
vector<vector<int>> levelOrderBottom(TreeNode *root) {
	vector<vector<int>> result;
	if (root == nullptr)
		return result;
	queue<Bucket> q;
	result.push_back(vector<int>());
	int curLevel = 0;
	q.push(Bucket(root, 0));
	while (!q.empty()) {
		Bucket bucket = q.front();
		q.pop();
		TreeNode *p = bucket.node;
		int level = bucket.level;
		if (level != curLevel) {
			result.push_back(vector<int>());
			curLevel++;
		}
		result[curLevel].push_back(p->val);
		if (p->left)
			q.push(Bucket(p->left, level + 1));
		if (p->right)
			q.push(Bucket(p->right, level + 1));
	}
	reverse(result.begin(), result.end());
}
```
