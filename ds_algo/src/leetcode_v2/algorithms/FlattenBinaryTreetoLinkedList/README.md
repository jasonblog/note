## Flatten Binary Tree to Linked List

Given a binary tree, flatten it to a linked list in-place.

For example,

Given
```

         1
        / \
       2   5
      / \   \
     3   4   6
```
The flattened tree should look like:
```
   1
    \
     2
      \
       3
        \
         4
          \
           5
            \
             6
```
Hints:

If you notice carefully in the flattened tree, each node's right child points to the next node of a pre-order traversal.

## Solution 1

递归方法，首先flatten左子树，然后flatten右子树，此时左子树和右子树都已经是一个链表了，
```
            1
           / \
          2   5
           \   \
            3   6
             \
              4
```
此时只需要合并左右子树即可,即把右子树接到左子树的最右节点，即把5接到4后面，最后把右子树指向左子树即可

** 处理完后，必须把左子树设为`NULL`，否则出现`RunTime ERROR` **

## Code
```cpp
void flatten(TreeNode *root) {
	if (root == nullptr)
		return ;
	flatten(root->left);
	flatten(root->right);
	if (root->right == nullptr) {
		root->right = root->left;
		root->left = nullptr; // 不能少这个，否则RE
		return;
	}
	TreeNode *p = root->left;
	if (p) {
		while (p->right) {
			p = p->right;
		}
		p->right = root->right;
		root->right = root->left;
	}
	root->left = nullptr; // 不能少，否则RE
}
```

## Solution 2

使用循环，从root开始，找到root的左子树的最右节点，然后把右子树接到最右节点，右子树指向左子树，左子树设为`NULL`，然后处理右子树即可

* `root == NULL`, 返回。否则`left = root->left`
* 若`left != NULL`, 找到left的最右子树p,
```
p->right = root->right;
root->right = root->left;
root->left = NULL;
```
* `root = root->right`, 回到最开始。

## Code
```cpp
void flatten(TreeNode *root) {
	while (root) {
		if (root->left) {
			TreeNode *left = root->left;
			while (left->right) left = left->right;
			left->right = root->right;
			root->right = root->left;
			root->left = nullptr;
		}
		root = root->right;
	}
}
```
