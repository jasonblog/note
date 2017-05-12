## Copy List with Random Pointer

A linked list is given such that each node contains an additional random pointer which could point to any node in the list or null.

Return a deep copy of the list. 

## Solution

关键问题是当拷贝random指针时有可能节点还没有生成。因此自然想到random指针先不理它。

于是我们先拷贝所有节点，即完成链表的deep copy， 但是random指针如何更新呢？

我们可以第二次扫描把把每个节点的一一映射关系放在map中，比如p为原始链表节点，q为新的链表的节点，`p1->q1, p2->q2`

第三次扫描更新random指针，根据p节点的random，在map中找到对应q的random, 比如`p1->random == p3, 则 q1->random == map[p3] == q3`

这样需要O(n)的空间。

有没有更优的方法？

我们可以先在原地插入相同的节点，形成一个新的链表，比如`1->2->3`，变成`1->1'->2->2'->3->3'`, 而random指针指向原来的值，即若`random(1) == 3, 则random(1') == 3`.

然后更新random，此时只需要更新第偶数个节点random指向下一个节点即可,即`random(p') = random(p')->next`

第三步，需要把拷贝的节点从原来的链表分离出来即可

总结出来，需要以下三步:

* 原地插入拷贝，random指向原来的节点
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

* 更新random指针，把第偶数个节点的random指向原来的下一个节点
```cpp
RandomListNode *updateRandomPointer(RandomListNode *head)
{
	if (head == nullptr)
		return nullptr;
	RandomListNode *p = head->next;
	while (p) {
		if (p->random)
			p->random = p->random->next;
		if (p->next == nullptr) // 最后一个节点next指针为null
			break;
		else
			p = p->next->next;
	}
	return head;
}

```
* 从原来的链表中分离开，返回复制的节点
```cpp
RandomListNode *breakDown(RandomListNode *head)
{
	if (head == nullptr)
		return nullptr;
	RandomListNode *head1 = head, *head2 = head->next;
	RandomListNode *p = head1, *q = head2;
	while (p && q) {
		p->next = p->next->next;
		if (q->next) // 最后一个节点指向null
			q->next = q->next->next;
		p = p->next;
		q = q->next;
	}
	return head2;
}
```

最后代码为:
```cpp
RandomListNode *copyRandomList(RandomListNode *head)
{
	copyInPlace(head);
	updateRandomPointer(head); // 不能和breakdown一起，原因是随机指针可能指向前面的节点
	return breakDown(head);
}

```

需要注意的是最后一个节点指向为null的情况，即`p->next->next`,注意最后一个节点的`p->next`为null，因此会出现段错误.
