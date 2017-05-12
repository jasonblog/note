## Remove Duplicates from Sorted List II

Given a sorted linked list, delete all nodes that have duplicate numbers, leaving only distinct numbers from the original list.

For example,
Given `1->2->3->3->4->4->5`, return `1->2->5`.
Given `1->1->1->2->3`, return `2->3`. 

## Solution

这个与[Remove Duplicates from Sorted List](../RemoveDuplicatesfromSortedList)不同，本题是要删除所有重复的元素，即重复的就要删除，一个不剩，而后者是要删除重复的元素，只保留一个元素.

设p为当前的元素，初始化为head, pre为前一个元素，初始化为null， q为后一个元素，即`q = p->next`

* 若`q->val != p->val`, 则直接更新p，pre

```c
pre = p;
p = p->next;
```

* 否则指向删除q，直到q与p值不相等
```c
while(q && q->val == p->val) {
	p->next = p->next->next;
	free(q);
	q = p->next;
}
```
然后执行以下操作:

+ 若`p == head`, 即头节点就出现重复，需要更新head指针，并释放p

```c
head = p->next;
free(p);
p = head;
```

+ 若`p != head`, 则只需要删除p节点即可，利用pre指针

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

## 扩展

[Remove Duplicates from Sorted List I](../RemoveDuplicatesfromSortedList), 删除重复的元素，即重复的只保留一个，相当于去重
