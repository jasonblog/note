## Insertion Sort List

Sort a linked list using insertion sort.

## Solution

插入排序的思想并不复杂，关键是和数组不一样，不能向前搜索插入位置，而必须向前搜索

假设链表头部指针head，prev为处理完毕的最后一个节点，即prev之前(包括prev)的节点是已经排好序的，p为当前节点

* 若`p->val >= prev->val`, 已经有序，更新prev = p
* 若`p->val <= head->val`, 说明p是当前最小的值，需要插入到头部, 并更新头部指针指向p
```c
prev->next = p->next;
p->next = head;
head = p;
p = prev->next;
```
* t从头部开始遍历直到,`t->next >= p->val`, 则p插入t之后即可
```c
struct ListNode *t = head;
while (t->next->val < p->val) {
	t = t->next;
}
prev->next = p->next;
p->next = t->next;
t->next = p;
p = prev->next;
```

## Code
```c
struct ListNode *insertionSortList(struct ListNode *head)
{
	if (head == NULL)
		return NULL;
	struct ListNode *p = head->next;
	struct ListNode *prev = head;
	while (p) {
		if (p->val >= prev->val) {
			prev = p;
			p = p->next;
			continue;
		}
		if (p->val <= head->val) {
			prev->next = p->next;
			p->next = head;
			head = p;
			p = prev->next;
		} else {
			struct ListNode *t = head;
			while (t->next->val < p->val) {
				t = t->next;
			}
			prev->next = p->next;
			p->next = t->next;
			t->next = p;
			p = prev->next;
			
		}
	}
	return head;
}
```
