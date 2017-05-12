## Populating Next Right Pointers in Each Node II

Follow up for problem "Populating Next Right Pointers in Each Node".

What if the given tree could be any binary tree? Would your previous solution still work?

Note:

* You may only use constant extra space.

For example,
Given the following binary tree,

```
         1
       /  \
      2    3
     / \    \
    4   5    7
```

After calling your function, the tree should look like:

```
         1 -> NULL
       /  \
      2 -> 3 -> NULL
     / \    \
    4-> 5 -> 7 -> NULL
```

## Solution

这一题思路有的，但就是做起来逻辑有点混乱，涉及指针如何移动。。。。多移一个少移一个。。。

这题和[上一题](../PopulatingNextRightPointersinEachNode)的思路是一样的，唯一变化的就是，这是一个任意的二叉树，而不是
满二叉树，也就是说`p->left->next` 不再一定指向`p->right`, 而要指向和它相邻的最近右节点(而不一定是兄弟节点)。问题便是如何找到这个右节点出来。

我们先定义一个函数`getNext(p)`，这个函数找到p节点的下一层的最左节点，同时p会移动到结果的父亲节点，即下图中

```
     p  ->   q
              \
              r2
               \
              r22

有:
getNext(p) == r2， 此时p == q， 因为原来的p没有孩子
getNext(q) == r2, 此时q不变，q有孩子
getNext(r2) == r22，r2不变，r2有孩子

```

代码为:

```cpp
TreeLinkNode *getNext(TreeLinkNode * &p) {
		if (p == nullptr)
			return nullptr;
		TreeLinkNode *next = p->left ? p->left : p->right;
		if (next)
			return next;
		p = p->next;
		return getNext(p);
	}
```

还是假设我们正在处理p的所有下一层节点，和p一个层次的以及p上面的节点都已经处理完了。

```
     p  ->   q
    /         \
   l1         r2
  /  \       /  \
 l11  l12   r21  r22
```

我们调用`t1 = getNext(p)`, 得到`t1 == l1`, 然后由于p没有右孩子， 我们调用`p = p->next; t2 = getNext(p)`, 得到`t2 == r2`, 此时只需要
设置t1的next指针指向t2即可,即`t1->next = t2`

```
     p  ->   q
    /         \
   l1   ->    r2
  /  \       /  \
 l11  l12   r21  r22
```

然后我们令`t1 = t2`, 继续调用直到getNext()返回null即可

```cpp

void connect(TreeLinkNode *root) {
	if (root == nullptr)
		return;
	TreeLinkNode *p = root;
	while (p) {
		TreeLinkNode *q = p;
		TreeLinkNode *t1 = getNext(q);
		TreeLinkNode *t2;
		while (t1) {
			if (t1 == q->left) { // 如果t1是左孩子
				t2 = q->right; // 令t2是它的右孩子，（可能为null）
			} else {
				t2 = nullptr; // 否则t1已经是右孩子，t2必须从下一个节点寻找
			}
			if (t2 == nullptr) {
				q = q->next; // 从下一个节点寻找t2
				t2 = getNext(q);
			}
			if (t2 == nullptr) // t2为null，说明没有找到最右相邻节点
				break;
			t1->next = t2; // 更新t1的next指针指向t2
			t1 = t2; 
		}
		p = getNext(p); // 处理下一层节点，注意下一层的最初节点就是p的下一层节点的最左节点，即getNext(p)
	}
}
```

## 扩展

[Populating Next Right Pointers in Each Node](../PopulatingNextRightPointersinEachNode)

## 总结

做这种题目，思路一定要清晰，尤其是各种指针，不能乱。。

另外这一题居然一次AC :)

后面看了答案，还可以这么Simple。..., 好吧。。。

```java
public void connect(TreeLinkNode root) {
	while(root != null){
	    TreeLinkNode tempChild = new TreeLinkNode(0);
	    TreeLinkNode currentChild = tempChild;
	    while(root!=null){
		if(root.left != null) { currentChild.next = root.left; currentChild = currentChild.next;}
		if(root.right != null) { currentChild.next = root.right; currentChild = currentChild.next;}
		root = root.next;
	    }
	    root = tempChild.next;
	}
}
```
