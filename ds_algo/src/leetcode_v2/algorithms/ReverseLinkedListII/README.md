## Reverse Linked List II

Reverse a linked list from position m to n. Do it in-place and in one-pass.

For example:
Given `1->2->3->4->5->NULL`, `m = 2` and `n = 4`,

return `1->4->3->2->5->NULL`.

Note:
Given `m, n` satisfy the following condition:
`1 ≤ m ≤ n ≤` length of list.

## Solution

先实现给定start和end逆转:

```cpp
void reverse(ListNode *start, ListNode *end) {
	if (!start || !start->next)
		return;
	ListNode *pre = nullptr;
	ListNode *p = start;
	while (p && pre != end) {
		ListNode *q = p->next;
		p->next = pre;
		pre = p;
		p = q;
	}
	start->next = p; // 注意最后需要更新start为end的next节点
}
```

然后只需要定位这个两个指针即可

```cpp
ListNode *start = head;
ListNode *end = head;
ListNode *prev = nullptr; // 保存前一个节点，因为reverse后需要指向end节点
while (--m) {
	prev = start;
	start = start->next;
	end = end->next;
	n--;
}
while (--n) {
	end = end->next;
}
```

调用`reverse(start, end)`即可

最后如果start是head节点，则需要更新head为end节点，否则更新prev节点指向end

```cpp
if (prev)
	prev->next = end;
else
	head = end;
```

## 扩展

[Reverse Linked List](../ReverseLinkedList): 逆转整个链表,实现了递归法和迭代法
