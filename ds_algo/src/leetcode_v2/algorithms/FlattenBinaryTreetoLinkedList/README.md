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

遞歸方法，首先flatten左子樹，然後flatten右子樹，此時左子樹和右子樹都已經是一個鏈表了，
```
            1
           / \
          2   5
           \   \
            3   6
             \
              4
```
此時只需要合併左右子樹即可,即把右子樹接到左子樹的最右節點，即把5接到4後面，最後把右子樹指向左子樹即可

** 處理完後，必須把左子樹設為`NULL`，否則出現`RunTime ERROR` **

## Code
```cpp
void flatten(TreeNode *root) {
	if (root == nullptr)
		return ;
	flatten(root->left);
	flatten(root->right);
	if (root->right == nullptr) {
		root->right = root->left;
		root->left = nullptr; // 不能少這個，否則RE
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
	root->left = nullptr; // 不能少，否則RE
}
```

## Solution 2

使用循環，從root開始，找到root的左子樹的最右節點，然後把右子樹接到最右節點，右子樹指向左子樹，左子樹設為`NULL`，然後處理右子樹即可

* `root == NULL`, 返回。否則`left = root->left`
* 若`left != NULL`, 找到left的最右子樹p,
```
p->right = root->right;
root->right = root->left;
root->left = NULL;
```
* `root = root->right`, 回到最開始。

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
