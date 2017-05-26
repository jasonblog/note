## Linked List Cycle

Given a linked list, determine if it has a cycle in it.

Follow up:
Can you solve it without using extra space? 

## Solution

快慢指針法

設兩個指針p, q, p每次移一個節點，q每次移動兩個節點，如果`p == q`，則有環。如果p,q其一等於`NULL`,則無環

更多鏈表環操作，見[Linked List Cycle II](../LinkedListCycle2)

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
