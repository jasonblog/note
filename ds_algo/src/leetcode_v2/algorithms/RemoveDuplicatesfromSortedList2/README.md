## Remove Duplicates from Sorted List II

Given a sorted linked list, delete all nodes that have duplicate numbers, leaving only distinct numbers from the original list.

For example,
Given `1->2->3->3->4->4->5`, return `1->2->5`.
Given `1->1->1->2->3`, return `2->3`. 

## Solution

這個與[Remove Duplicates from Sorted List](../RemoveDuplicatesfromSortedList)不同，本題是要刪除所有重複的元素，即重複的就要刪除，一個不剩，而後者是要刪除重複的元素，只保留一個元素.

設p為當前的元素，初始化為head, pre為前一個元素，初始化為null， q為後一個元素，即`q = p->next`

* 若`q->val != p->val`, 則直接更新p，pre

```c
pre = p;
p = p->next;
```

* 否則指向刪除q，直到q與p值不相等
```c
while(q && q->val == p->val) {
	p->next = p->next->next;
	free(q);
	q = p->next;
}
```
然後執行以下操作:

+ 若`p == head`, 即頭節點就出現重複，需要更新head指針，並釋放p

```c
head = p->next;
free(p);
p = head;
```

+ 若`p != head`, 則只需要刪除p節點即可，利用pre指針

```c
pre->next = pre->next->next;
free(p);
p = q;
```

## Code
```c
struct ListNode* deleteDuplicates(struct ListNode *head)
{
	struct ListNode *p = head, *pre = NULL;
	while (p) {
		struct ListNode *q = p->next; // q is the next node of p
		bool isDup = false;
		while (q && q->val == p->val) {
			isDup = true;
			// delete q
			p->next = p->next->next;
			free(q);
			// update q
			q = p->next;
		}
		if (isDup) {
			if (p == head) {
				head = p->next;
				free(p);
				p = head;
			} else {
				pre->next = pre->next->next;
				free(p);
				p = q;
			}
		} else {
			pre = p;
			p = p->next;;
		}
	}
	return head;
}
```

## 擴展

[Remove Duplicates from Sorted List I](../RemoveDuplicatesfromSortedList), 刪除重複的元素，即重複的只保留一個，相當於去重
