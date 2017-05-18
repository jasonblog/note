## Swap Nodes in Pairs

Given a linked list, swap every two adjacent nodes and return its head.

For example,
```
Given 1->2->3->4, you should return the list as 2->1->4->3.
```

Your algorithm should use only constant space. You may not modify the values in the list, only nodes itself can be changed. 

## Solution

用三个指针p, q, pre, p指向当前节点，初始化为head，q指向p的下一个节点，pre指向上次完成的节点，初始化null

交换p，q，同时需要修改pre的下一个节点(之前指向p，由于p和q交换位置，需要修改指向q):

```cpp
while (p && p->next) {
	ListNode *q = p->next;
	p->next = q->next;
	q->next = p;
	if (pre)
		pre->next = q;
	pre = p;
	p = p->next;
}
```

同时需要注意更新head指针。完整代码:

```cpp
ListNode *swapPairs(ListNode *head) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *p = head;
	head = p->next;
	ListNode *pre = nullptr;
	while (p && p->next) {
		ListNode *q = p->next;
		p->next = q->next;
		q->next = p;
		if (pre)
			pre->next = q;
		pre = p;
		p = p->next;
	}
	return head;
}
```

## 扩展

[Reverse Nodes in k-Group](../ReverseNodesink-Group)
