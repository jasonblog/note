## Linked List Cycle

Given a linked list, determine if it has a cycle in it.

Follow up:
Can you solve it without using extra space? 

## Solution

快慢指针法

设两个指针p, q, p每次移一个节点，q每次移动两个节点，如果`p == q`，则有环。如果p,q其一等于`NULL`,则无环

更多链表环操作，见[Linked List Cycle II](../LinkedListCycle2)

## Code
```c
bool hasCycle(struct ListNode *head) {
	if (head == NULL)
	    return false;
	struct ListNode *p = head, *q = head->next;;
	while (p && q) {
	    if (p == q)
		    return true;
	    p = p -> next;
	    q = q -> next;
	    if (q == NULL)
		    return false;
	    q = q -> next;
	}
}
```
