## Reorder List

Given a singly linked list L: `L0→L1→…→Ln-1→Ln`,
reorder it to: `L0→Ln→L1→Ln-1→L2→Ln-2→…`

You must do this in-place without altering the nodes' values.

For example,
Given {1,2,3,4}, reorder it to {1,4,2,3}.

## Solution

首先把链表分成左右两半，若节点个数是奇数，则左边节点个数比右边节点个数多一个节点.

使用快慢指针法，找到链表的中间节点

```cpp
ListNode *getMidNode(ListNode *head) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *slow = head, *fast = head;
	while (fast != nullptr) {
		slow = slow->next;
		fast = fast->next;
		if (fast)
			fast = fast->next;
	}
	return slow;
}
```

然后把右半部分逆转,实现逆转链表方法:

```cpp
ListNode *reverse(ListNode *head) {
	ListNode *prev = nullptr;
	ListNode *p = head;
	while (p) {
		ListNode *q = p->next;
		p->next = prev;
		prev = p;
		p = q;
	}
	return prev;
}
```

最后把左右两半指针间隔插入归并即可:

```cpp
void merge(ListNode *left, ListNode *right) {
	ListNode *p = left, *q = right;
	while (p && q) {
		ListNode *nextLeft = p->next;
		ListNode *nextRight = q->next;
		p->next = q;
		q->next = nextLeft;
		p = nextLeft;
		q = nextRight;
	}
	if (p)
		p->next = nullptr;
}
```

整个代码流程为:

```cpp
void reorderList(ListNode *head) {
	ListNode *mid = getMidNode(head);
	ListNode *left = head;
	ListNode *right = reverse(mid); 
	merge(left, right);
}
```
