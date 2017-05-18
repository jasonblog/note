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

层次遍历，若当前节点和下一个节点的层次相同，则连接当前节点指向下一个节点

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
			if (!q.empty()) { // 注意检查是否还存在节点
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

但以上方法需要一个队列，需要O(n)的空间。题目要求O(1)的空间。如何解决呢？

首先题目已经说明给定的是满二叉树，即所有的叶子都在同一层，并且除了叶子节点，都有两个孩子节点，不存在单孩子节点。

假设我们已经处理完某层的p，现在处理p以下的所有节点，并且上面的层次都已经处理完了，我们可以充分利用next指针来完成。

* 首先`p->left->next = p->right`，这是显然的，每个左孩子必然指向它的右孩子.

```
    p ->  q
   / \   / \
  l1 r1 l2 r2
```

* 那么`p->right->next` 指向哪个节点呢？显然是与p同层次的右兄弟节点的左孩子。
如何得到p的右兄弟？因为p层已经处理完毕，则`p->next`就是它的右兄弟,于是显然`p->right->next = p->next->left`
* 然后处理同层次的下一个节点的孩子，`p = p->next`，此时p为q节点， 直到p为null，即没有右兄弟节点为止.
* 处理完成该层次后，处理下一个层次，即`p = pre->left`, pre表示该成的最左节点，即p为上图的l1节点。

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

## 扩展

[Populating Next Right Pointers in Each Node II](../PopulatingNextRightPointersinEachNodeII), 二叉树不是满二叉树的情况，甚至不是完全二叉树！
