## Copy List with Random Pointer

A linked list is given such that each node contains an additional random pointer which could point to any node in the list or null.

Return a deep copy of the list. 

## Solution

關鍵問題是當拷貝random指針時有可能節點還沒有生成。因此自然想到random指針先不理它。

於是我們先拷貝所有節點，即完成鏈表的deep copy， 但是random指針如何更新呢？

我們可以第二次掃描把把每個節點的一一映射關係放在map中，比如p為原始鏈表節點，q為新的鏈表的節點，`p1->q1, p2->q2`

第三次掃描更新random指針，根據p節點的random，在map中找到對應q的random, 比如`p1->random == p3, 則 q1->random == map[p3] == q3`

這樣需要O(n)的空間。

有沒有更優的方法？

我們可以先在原地插入相同的節點，形成一個新的鏈表，比如`1->2->3`，變成`1->1'->2->2'->3->3'`, 而random指針指向原來的值，即若`random(1) == 3, 則random(1') == 3`.

然後更新random，此時只需要更新第偶數個節點random指向下一個節點即可,即`random(p') = random(p')->next`

第三步，需要把拷貝的節點從原來的鏈表分離出來即可

總結出來，需要以下三步:

* 原地插入拷貝，random指向原來的節點
```cpp
RandomListNode *copyInPlace(RandomListNode *head)
{
	if (head == nullptr)
		return nullptr;
	RandomListNode *p = head;
	while (p) {
		RandomListNode *t = new RandomListNode(p->label);
		t->next = p->next;
		t->random = p->random;
		p->next = t;
		p = p->next->next;
	}
	return head;
}
```

* 更新random指針，把第偶數個節點的random指向原來的下一個節點
```cpp
RandomListNode *updateRandomPointer(RandomListNode *head)
{
	if (head == nullptr)
		return nullptr;
	RandomListNode *p = head->next;
	while (p) {
		if (p->random)
			p->random = p->random->next;
		if (p->next == nullptr) // 最後一個節點next指針為null
			break;
		else
			p = p->next->next;
	}
	return head;
}

```
* 從原來的鏈表中分離開，返回複製的節點
```cpp
RandomListNode *breakDown(RandomListNode *head)
{
	if (head == nullptr)
		return nullptr;
	RandomListNode *head1 = head, *head2 = head->next;
	RandomListNode *p = head1, *q = head2;
	while (p && q) {
		p->next = p->next->next;
		if (q->next) // 最後一個節點指向null
			q->next = q->next->next;
		p = p->next;
		q = q->next;
	}
	return head2;
}
```

最後代碼為:
```cpp
RandomListNode *copyRandomList(RandomListNode *head)
{
	copyInPlace(head);
	updateRandomPointer(head); // 不能和breakdown一起，原因是隨機指針可能指向前面的節點
	return breakDown(head);
}

```

需要注意的是最後一個節點指向為null的情況，即`p->next->next`,注意最後一個節點的`p->next`為null，因此會出現段錯誤.
