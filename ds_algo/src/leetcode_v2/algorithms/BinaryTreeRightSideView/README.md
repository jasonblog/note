## Binary Tree Right Side View

Given a binary tree, imagine yourself standing on the right side of it, return the values of the nodes you can see ordered from top to bottom.

For example:
Given the following binary tree,

```

   1            <---
 /   \
2     3         <---
 \     \
  5     4       <---
```

You should return `[1, 3, 4]`. 

## Solution

求每一层的最右的一个节点，层次遍历树，每一层的最后一个节点就是结果元素

使用一个桶封装树节点，加上层次，当遍历层次更新时，上一个节点就是上一个层次的最后一个节点

## Code
```cpp
typedef struct bucket{
	TreeNode *node;
	int level;
	bucket(TreeNode *n, int l):node(n), level(l){}
} bucket, *Bucket;
class Solution {
public:
    vector<int> rightSideView(TreeNode *root) {
	    vector<int> result;
	    if (root == nullptr)
		    return result;
	    queue<bucket> q;
	    q.push(bucket(root, 0));
	    int level = 0;
	    while (!q.empty()) {
		    auto b = q.front();
		    q.pop();
		    auto p = b.node;
		    if (p->left)
			    q.push(bucket(p->left, level + 1));
		    if (p->right)
			    q.push(bucket(p->right, level + 1));
		    if (q.empty() || q.front().level != level) {
			    result.push_back(p->val);
			    level = q.front().level;
		    }
	    }
	    return result;
    }
};
```
