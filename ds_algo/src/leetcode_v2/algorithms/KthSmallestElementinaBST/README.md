## Kth Smallest Element in a BST

Given a binary search tree, write a function `kthSmallest` to find the kth smallest element in it.

Note: 
You may assume k is always valid, 1 ≤ k ≤ BST's total elements.

Follow up:
What if the BST is modified (insert/delete operations) often and you need to find the kth smallest frequently? How would you optimize the kthSmallest routine?

Hint:

* Try to utilize the property of a BST.
* What if you could modify the BST node's structure?
* The optimal runtime complexity is O(height of BST).

Credits:
Special thanks to @ts for adding this problem and creating all test cases.

## Solution

中序遍历，更新k值，访问时`k--`， 若此时`k == 0`, 则当前值即为结果

```cpp
public:
	int kthSmallest(TreeNode *root, int k) {
		int result;
		/*
		if (root == nullptr) {
			return 0;
		}
		*/
		kthSmallest(root, k, result);
		return result;
	}
private:
	/* k为需要查找的第k小的数， result为结果，返回true表示已经查找到
	 * 注意k为引用类型，否则k的值更新不能反馈到上一栈调用
	*/
	bool kthSmallest(TreeNode *root, int &k, int &result) {
		if (root->left) {
			if (kthSmallest(root->left, k, result)) {
				return true;
			}
		}
		k -= 1;
		if (k == 0) {
			result = root->val;
			return true;
		}
		if (root->right) {
			if (kthSmallest(root->right, k, result)) {
				return true;
			}
		}
		return false;
	}
};
```

## 优化

如果允许修改树结构，保存节点左孩子个数，是否可以优化呢？
