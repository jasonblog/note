## Reverse Linked List

Reverse a singly linked list.

click to show more hints.
Hint:

A linked list can be reversed either iteratively or recursively. Could you implement both?

## Solution

迭代法:

```cpp
ListNode *reverseIteratively(ListNode *head) {
	if (!head || !head->next)
		return head;
	ListNode *prev = nullptr, *p = head;
	while (p) {
		ListNode *q = p->next;
		p->next = prev;
		prev = p;
		p = q;
	}
	return prev;
}
```

遞歸法，注意更新head指針:

```cpp
ListNode *reverseRecursively(ListNode *&head, ListNode *p) {
	if (!p|| !p->next) {
		head = p;
		return head;
	}
	reverseRecursively(head, p->next)->next = p;
	p->next = nullptr;
	return p;
}
```

## 擴展

[Reverse Linked List II](../ReverseLinkedListII): 逆轉指定的區間
