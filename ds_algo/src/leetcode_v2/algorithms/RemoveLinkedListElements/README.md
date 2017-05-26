## Remove Linked List Elements

Remove all elements from a linked list of integers that have value val.

Example
```
Given: 1 --> 2 --> 6 --> 3 --> 4 --> 5 --> 6, val = 6
Return: 1 --> 2 --> 3 --> 4 --> 5
```

Credits:
Special thanks to @mithmatt for adding this problem and creating all test cases.

## Solution

* 先處理頭部，如果頭部就是要刪除的值，直接讓頭部指向下一個節點，並釋放原來的頭部
```c
while (head && head->val == val) {
	struct ListNode *q = p->next;
	free(head);
	head = q;
}
```
* 處理非頭部節點，此時無需更新頭部，只需要遍歷等於刪除值的節點即可。為了便於刪除操作，判斷後一個節點，若後一個節點值為刪除值，則只需要刪除後一個節點即可，否則指向下一個節點
```c
struct ListNode *p = head;
while (p) {
	struct ListNode *q = p->next;
	if (q && q->val == val) {
		p->next = q->next;
		free(q);
	} else {
		p = p->next;
	}
}
```

## Code
```c
struct ListNode *removeElements(struct ListNode *head, int val)
{
	struct ListNode *p =head;
	while (p && p->val == val) {
		struct ListNode *q = p->next;
		free(p);
		p = q;
		head = q;
	}
	while (p) {
		struct ListNode *q = p->next;
		if (q && q->val == val) {
			p->next = q->next;
			free(q);
		} else {
			p = p->next;
		}
	}
	return head;
}
```
