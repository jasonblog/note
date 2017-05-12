## Remove Duplicates from Sorted List

Given a sorted linked list, delete all duplicates such that each element appear only once.

For example,
Given `1->1->2`, return `1->2`.
Given `1->1->2->3->3`, return `1->2->3`. 

## Solution

设p指向当前节点，初始化为`p = head`, `q = p->next`, 则:

* 若q为null，则说明已到达尾部节点，直接返回
* 若`q->val != p->val`, 更新p节点, `p = p->next`
* 否则删除q节点，即`p->next = p->next->next; free(q)`

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

## 扩展

删除所有的重复节点[Remove Duplicates from Sorted List II](../RemoveDuplicatesfromSortedList2)
