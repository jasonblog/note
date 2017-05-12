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

首先定义一个reverse函数，接收两个指针，分别为begin和end，表示reverse的开始和结束，同时由于end的next指针会丢失，因此应该预先保存并返回

```cpp
ListNode *reverse(ListNode *begin, ListNode *end) {
	assert(begin && end);
	ListNode *p = begin;
	ListNode *prev = nullptr;
	ListNode *endFlag = end->next;
	while (p != endFlag) { // 条件不能写成 p != end->next, 因为end->next reverse时会修改
		ListNode *q = p->next; // 保存next指针
		p->next = prev;
		prev = p;
		p = q;
	}
	return p;
}
```

然后分组，思路是每次走k步，找到需要reverse的开始和结束节点，同时注意保存之前已经处理好的尾部节点prev

```cpp
while (begin) {
	int K = k - 1; // 包括begin节点，因此后面还需要k-1个节点
	ListNode *p = begin;
	while (K && p->next) {
		end = p->next;
		p = p->next;
		K--;
	}
	if (K) { // 不足k，不需要reverse，但注意需要更新尾部节点
		prev->next = begin; // 记得最后更新prev指针
		return head;
	}
	ListNode *next = reverse(begin, end);
	prev->next = end; // 更新已经处理完毕的尾部节点
	prev = begin; // 更新处理完毕的尾部节点指针
	begin = next; // begin从下一个节点开始
}
```

完整代码:

```cpp
ListNode *reverseKGroup(ListNode *head, int k) {
	if (head == nullptr || k <= 1)
		return head;
	ListNode *begin = head, *end = nullptr;
	ListNode *prev = nullptr;
	/* First reverse */
	if (begin) { // 第一次操作，需要更新head和prev指针
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
			prev->next = begin; // 记得最后更新prev指针
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
