## Insertion Sort List

Sort a linked list using insertion sort.

## Solution

插入排序的思想並不複雜，關鍵是和數組不一樣，不能向前搜索插入位置，而必須向前搜索

假設鏈表頭部指針head，prev為處理完畢的最後一個節點，即prev之前(包括prev)的節點是已經排好序的，p為當前節點

* 若`p->val >= prev->val`, 已經有序，更新prev = p
* 若`p->val <= head->val`, 說明p是當前最小的值，需要插入到頭部, 並更新頭部指針指向p
```c
prev->next = p->next;
p->next = head;
head = p;
p = prev->next;
```
* t從頭部開始遍歷直到,`t->next >= p->val`, 則p插入t之後即可
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
