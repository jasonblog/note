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

這一題思路有的，但就是做起來邏輯有點混亂，涉及指針如何移動。。。。多移一個少移一個。。。

這題和[上一題](../PopulatingNextRightPointersinEachNode)的思路是一樣的，唯一變化的就是，這是一個任意的二叉樹，而不是
滿二叉樹，也就是說`p->left->next` 不再一定指向`p->right`, 而要指向和它相鄰的最近右節點(而不一定是兄弟節點)。問題便是如何找到這個右節點出來。

我們先定義一個函數`getNext(p)`，這個函數找到p節點的下一層的最左節點，同時p會移動到結果的父親節點，即下圖中

```
     p  ->   q
              \
              r2
               \
              r22

有:
getNext(p) == r2， 此時p == q， 因為原來的p沒有孩子
getNext(q) == r2, 此時q不變，q有孩子
getNext(r2) == r22，r2不變，r2有孩子

```

代碼為:

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

還是假設我們正在處理p的所有下一層節點，和p一個層次的以及p上面的節點都已經處理完了。

```
     p  ->   q
    /         \
   l1         r2
  /  \       /  \
 l11  l12   r21  r22
```

我們調用`t1 = getNext(p)`, 得到`t1 == l1`, 然後由於p沒有右孩子， 我們調用`p = p->next; t2 = getNext(p)`, 得到`t2 == r2`, 此時只需要
設置t1的next指針指向t2即可,即`t1->next = t2`

```
     p  ->   q
    /         \
   l1   ->    r2
  /  \       /  \
 l11  l12   r21  r22
```

然後我們令`t1 = t2`, 繼續調用直到getNext()返回null即可

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
				t2 = q->right; // 令t2是它的右孩子，（可能為null）
			} else {
				t2 = nullptr; // 否則t1已經是右孩子，t2必須從下一個節點尋找
			}
			if (t2 == nullptr) {
				q = q->next; // 從下一個節點尋找t2
				t2 = getNext(q);
			}
			if (t2 == nullptr) // t2為null，說明沒有找到最右相鄰節點
				break;
			t1->next = t2; // 更新t1的next指針指向t2
			t1 = t2; 
		}
		p = getNext(p); // 處理下一層節點，注意下一層的最初節點就是p的下一層節點的最左節點，即getNext(p)
	}
}
```

## 擴展

[Populating Next Right Pointers in Each Node](../PopulatingNextRightPointersinEachNode)

## 總結

做這種題目，思路一定要清晰，尤其是各種指針，不能亂。。

另外這一題居然一次AC :)

後面看了答案，還可以這麼Simple。..., 好吧。。。

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
