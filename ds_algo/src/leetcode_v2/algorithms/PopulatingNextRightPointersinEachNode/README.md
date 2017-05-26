## Populating Next Right Pointers in Each Node

Given a binary tree

```cpp
    struct TreeLinkNode {
      TreeLinkNode *left;
      TreeLinkNode *right;
      TreeLinkNode *next;
    }
```
Populate each next pointer to point to its next right node. If there is no next right node, the next pointer should be set to `NULL`.

Initially, all next pointers are set to `NULL`.

Note:

* You may only use constant extra space.
* You may assume that it is a perfect binary tree (ie, all leaves are at the same level, and every parent has two children).

For example,
Given the following perfect binary tree,

```
         1
       /  \
      2    3
     / \  / \
    4  5  6  7
```

After calling your function, the tree should look like:

```
         1 -> NULL
       /  \
      2 -> 3 -> NULL
     / \  / \
    4->5->6->7 -> NULL
```

## Solution

層次遍歷，若當前節點和下一個節點的層次相同，則連接當前節點指向下一個節點

```cpp
struct Bucket {
	TreeLinkNode *node;
	int level;
	Bucket(TreeLinkNode *p, int l) : node(p), level(l){}
};
class Solution {
public:
	void connect(TreeLinkNode *root) {
		if (root == nullptr)
			return;
		queue<Bucket> q;
		q.push(Bucket(root, 0));
		while (!q.empty()) {
			Bucket current = q.front();
			TreeLinkNode *p = current.node;
			int level = current.level;
			q.pop();
			if (!q.empty()) { // 注意檢查是否還存在節點
				Bucket next = q.front();
				if (current.level == next.level) {
					p->next = next.node;
				}
			}
			if (p->left)
				q.push(Bucket(p->left, level + 1));
			if (p->right)
				q.push(Bucket(p->right, level + 1));
		}
	}
};
```

但以上方法需要一個隊列，需要O(n)的空間。題目要求O(1)的空間。如何解決呢？

首先題目已經說明給定的是滿二叉樹，即所有的葉子都在同一層，並且除了葉子節點，都有兩個孩子節點，不存在單孩子節點。

假設我們已經處理完某層的p，現在處理p以下的所有節點，並且上面的層次都已經處理完了，我們可以充分利用next指針來完成。

* 首先`p->left->next = p->right`，這是顯然的，每個左孩子必然指向它的右孩子.

```
    p ->  q
   / \   / \
  l1 r1 l2 r2
```

* 那麼`p->right->next` 指向哪個節點呢？顯然是與p同層次的右兄弟節點的左孩子。
如何得到p的右兄弟？因為p層已經處理完畢，則`p->next`就是它的右兄弟,於是顯然`p->right->next = p->next->left`
* 然後處理同層次的下一個節點的孩子，`p = p->next`，此時p為q節點， 直到p為null，即沒有右兄弟節點為止.
* 處理完成該層次後，處理下一個層次，即`p = pre->left`, pre表示該成的最左節點，即p為上圖的l1節點。

```cpp
void connect(TreeLinkNode *root) {
	if (root == nullptr)
		return;
	TreeLinkNode *p = root;
	while (p->left) {
		TreeLinkNode *q = p;
		while (q) {
			q->left->next = q->right;
			if (q->next) {
				q->right->next = q->next->left;
			}
				q = q->next;
		}
		p = p->left;
	}
}
```

## 擴展

[Populating Next Right Pointers in Each Node II](../PopulatingNextRightPointersinEachNodeII), 二叉樹不是滿二叉樹的情況，甚至不是完全二叉樹！
