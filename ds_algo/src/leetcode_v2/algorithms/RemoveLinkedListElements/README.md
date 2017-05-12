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

* 先处理头部，如果头部就是要删除的值，直接让头部指向下一个节点，并释放原来的头部
```c
while (head && head->val == val) {
	struct ListNode *q = p->next;
	free(head);
	head = q;
}
```
* 处理非头部节点，此时无需更新头部，只需要遍历等于删除值的节点即可。为了便于删除操作，判断后一个节点，若后一个节点值为删除值，则只需要删除后一个节点即可，否则指向下一个节点
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
