## Reverse Linked List II

Reverse a linked list from position m to n. Do it in-place and in one-pass.

For example:
Given `1->2->3->4->5->NULL`, `m = 2` and `n = 4`,

return `1->4->3->2->5->NULL`.

Note:
Given `m, n` satisfy the following condition:
`1 ≤ m ≤ n ≤` length of list.

## Solution

先實現給定start和end逆轉:

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
	start->next = p; // 注意最後需要更新start為end的next節點
}
```

然後只需要定位這個兩個指針即可

```cpp
ListNode *start = head;
ListNode *end = head;
ListNode *prev = nullptr; // 保存前一個節點，因為reverse後需要指向end節點
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

調用`reverse(start, end)`即可

最後如果start是head節點，則需要更新head為end節點，否則更新prev節點指向end

```cpp
if (prev)
	prev->next = end;
else
	head = end;
```

## 擴展

[Reverse Linked List](../ReverseLinkedList): 逆轉整個鏈表,實現了遞歸法和迭代法
