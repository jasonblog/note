## Remove Duplicates from Sorted List

Given a sorted linked list, delete all duplicates such that each element appear only once.

For example,
Given `1->1->2`, return `1->2`.
Given `1->1->2->3->3`, return `1->2->3`. 

## Solution

設p指向當前節點，初始化為`p = head`, `q = p->next`, 則:

* 若q為null，則說明已到達尾部節點，直接返回
* 若`q->val != p->val`, 更新p節點, `p = p->next`
* 否則刪除q節點，即`p->next = p->next->next; free(q)`

```c
struct ListNode* deleteDuplicates(struct ListNode *head)
{
	struct ListNode *p = head;
	while (p) {
		struct ListNode *q = p->next;
		if (q && p->val == q->val) {
			p->next = p->next->next;
			free(q);
		} else
			p = p->next;
	}
	return head;
}
```

## 擴展

刪除所有的重複節點[Remove Duplicates from Sorted List II](../RemoveDuplicatesfromSortedList2)
