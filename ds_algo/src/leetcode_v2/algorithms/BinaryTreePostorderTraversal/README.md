## Binary Tree Postorder Traversal

Given a binary tree, return the postorder traversal of its nodes' values.

For example:
Given binary tree {1,#,2,3},
```
   1
    \
     2
    /
   3
```
return [3,2,1].

## Solution 1

递归法, 这个方法没有什么难度

## Solution 2

前序遍历，然后把结果reverse

## Solution 3

根据后序遍历的方式，最先访问的是左孩子节点的最左的最后一个节点,比如

```

              1
             / \
            2   3
           / \
          5   4
	   \   \
            6  7
```
显然最先访问的是6，因此需要把之前的节点压入栈。
```cpp
void pushLeftOrRight(TreeNode *p, stack<TreeNode*> &s) {
	while (p) {
		s.push(p);
		p = p->left != nullptr ? p->left : p->right;
	}
}
```

显然先是从root开始，压入的顺序依次为`1 2 5 6`

然后依次出栈就是访问顺序，当出栈前需要判断是否还有右孩子没有访问

比如当前已访问2(2还没有出栈)，应该开始访问右孩子, 即调用`pushLeftOrRight(p->right, s)`

那怎么知道有没有访问右孩子呢？我们发现我们有两次机会会到达2，一次是访问完左孩子，一次是访问完右孩子，即访问完5和4

我们可以发现，只要2出栈前，看之前访问的是否是左孩子即可，若是左孩子，说明右孩子没有访问
```cpp
vector<int> postorderTraversal(TreeNode *root) {
	vector<int> result;
	if (root == nullptr)
		return result;
	stack<TreeNode*>s;
	pushLeftOrRight(root, s);
	while (!s.empty()) {
		TreeNode *p = getAndPop(s); // s.top(), s.pop()
		result.push_back(p->val);
		TreeNode *next = s.empty() ? nullptr : s.top();
		if (next && next->left == p)
			pushLeftOrRight(next->right, s);
	}
	return result;
}
```
