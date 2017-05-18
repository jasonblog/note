## Binary Tree Level Order Traversal

Given a binary tree, return the level order traversal of its nodes' values. (ie, from left to right, level by level).

For example:
Given binary tree `{3,9,20,#,#,15,7}`,
```
    3
   / \
  9  20
    /  \
   15   7
```
return its level order traversal as:
```
[
  [3],
  [9,20],
  [15,7]
]
```
confused what "`{1,#,2,3}`" means? > read more on how binary tree is serialized on OJ.

## Solution


层次遍历，使用队列。

需要保存层次，因此封装了一个桶用于保存处于的层次
```cpp
vector<vector<int>> levelOrder(TreeNode *root) {
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
	return result;
}
```
