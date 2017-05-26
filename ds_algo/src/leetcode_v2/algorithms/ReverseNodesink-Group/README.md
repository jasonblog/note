## Reverse Nodes in k-Group

Given a linked list, reverse the nodes of a linked list k at a time and return its modified list.

If the number of nodes is not a multiple of k then left-out nodes in the end should remain as it is.

You may not alter the values in the nodes, only nodes itself may be changed.

Only constant memory is allowed.

For example,

```
Given this linked list: 1->2->3->4->5

For k = 2, you should return: 2->1->4->3->5

For k = 3, you should return: 3->2->1->4->5 
```

## Solution

首先定義一個reverse函數，接收兩個指針，分別為begin和end，表示reverse的開始和結束，同時由於end的next指針會丟失，因此應該預先保存並返回

```cpp
ListNode *reverse(ListNode *begin, ListNode *end) {
	assert(begin && end);
	ListNode *p = begin;
	ListNode *prev = nullptr;
	ListNode *endFlag = end->next;
	while (p != endFlag) { // 條件不能寫成 p != end->next, 因為end->next reverse時會修改
		ListNode *q = p->next; // 保存next指針
		p->next = prev;
		prev = p;
		p = q;
	}
	return p;
}
```

然後分組，思路是每次走k步，找到需要reverse的開始和結束節點，同時注意保存之前已經處理好的尾部節點prev

```cpp
while (begin) {
	int K = k - 1; // 包括begin節點，因此後面還需要k-1個節點
	ListNode *p = begin;
	while (K && p->next) {
		end = p->next;
		p = p->next;
		K--;
	}
	if (K) { // 不足k，不需要reverse，但注意需要更新尾部節點
		prev->next = begin; // 記得最後更新prev指針
		return head;
	}
	ListNode *next = reverse(begin, end);
	prev->next = end; // 更新已經處理完畢的尾部節點
	prev = begin; // 更新處理完畢的尾部節點指針
	begin = next; // begin從下一個節點開始
}
```

完整代碼:

```cpp
ListNode *reverseKGroup(ListNode *head, int k) {
	if (head == nullptr || k <= 1)
		return head;
	ListNode *begin = head, *end = nullptr;
	ListNode *prev = nullptr;
	/* First reverse */
	if (begin) { // 第一次操作，需要更新head和prev指針
		int K = k - 1;
		ListNode *p = begin;
		while (K && p->next) {
			end = p->next;
			p = p->next;
			K--;
		}
		if (K)
			return head;
		ListNode *next = reverse(begin, end);
		prev = begin;
		head = end;
		begin = next;
	}
	while (begin) {
		int K = k - 1;
		ListNode *p = begin;
		while (K && p->next) {
			end = p->next;
			p = p->next;
			K--;
		}
		if (K) {
			prev->next = begin; // 記得最後更新prev指針
			return head;
		}
		ListNode *next = reverse(begin, end);
		prev->next = end;
		prev = begin;
		begin = next;
	}
	return head;
}
```

## 得瑟

居然一次AC，:)
