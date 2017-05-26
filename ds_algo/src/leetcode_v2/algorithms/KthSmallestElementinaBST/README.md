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

中序遍歷，更新k值，訪問時`k--`， 若此時`k == 0`, 則當前值即為結果

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
	/* k為需要查找的第k小的數， result為結果，返回true表示已經查找到
	 * 注意k為引用類型，否則k的值更新不能反饋到上一棧調用
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

## 優化

如果允許修改樹結構，保存節點左孩子個數，是否可以優化呢？
