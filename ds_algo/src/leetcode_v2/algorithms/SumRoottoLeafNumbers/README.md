## Sum Root to Leaf Numbers

Given a binary tree containing digits from 0-9 only, each root-to-leaf path could represent a number.

An example is the root-to-leaf path 1->2->3 which represents the number 123.

Find the total sum of all root-to-leaf numbers.

For example,
```
    1
   / \
  2   3
```
The root-to-leaf path `1->2` represents the number `12`.
The root-to-leaf path `1->3` represents the number `13`.

Return the `sum = 12 + 13 = 25`. 

## Solution

递归前序遍历树，并记录经过的数值，当到达叶子时，保存值到vector中, 然后求vector的值即可.
```cpp
static inline bool isLeaf(TreeNode *root)
{
	return root->left == NULL && root->right == NULL;
}
class Solution {
public:
	int sumNumbers(TreeNode *root) {
		if (root == NULL)
			return 0;
		vector<int> numbers;
		getNumbers(numbers, root, 0);
		int sum = 0;
		for_each(numbers.begin(), numbers.end(), [&sum](int value){sum += value;});
		return sum;
	}
private:
	void getNumbers(vector<int> &result, TreeNode *root, int number) {
		if (root == NULL)
			return;
		number *= 10;
		number += root->val;
		if (isLeaf(root)) {
			result.push_back(number);
		} else {
			getNumbers(result, root->left, number);
			getNumbers(result, root->right, number);
		}
	}
};
```

以上方法的时间空间复杂度都是O(n)。

时间是不可能优化的，关键空间，我们可以不保存值，直接求解。
```cpp
class Solution {
public:
	int sumNumbers(TreeNode *root) {
		return sumNumbers(root, 0);
	}
private:
	int sumNumbers(TreeNode *root, int sum) {
		if (root == NULL)
			return 0;
		sum = sum * 10 + root->val;
		if (isLeaf(root)) {
			return sum;
		} else {
			return sumNumbers(root->left, sum) + sumNumbers(root->right, sum);
		}
	}
};
```

此时空间复杂度为O(1).
