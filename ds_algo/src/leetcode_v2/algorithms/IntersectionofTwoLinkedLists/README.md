## Intersection of Two Linked Lists

Write a program to find the node at which the intersection of two singly linked lists begins.

For example, the following two linked lists:
```
A:          a1 → a2
                   ↘
                     c1 → c2 → c3
                   ↗            
B:     b1 → b2 → b3
```
begin to intersect at node c1.

Notes:

    * If the two linked lists have no intersection at all, return null.
    * The linked lists must retain their original structure after the function returns.
    * You may assume there are no cycles anywhere in the entire linked structure.
    * Your code should preferably run in O(n) time and use only O(1) memory.

Credits:
Special thanks to @stellari for adding this problem and creating all test cases.

## Solution

設兩個鏈表的長度之差為diff, 設兩個指針p, q, p指向長的鏈表頭節點，q指向短鏈表頭指針。
先讓p先走diff步，然後p、q一起走，直到`p == q`，此時即找到了相交節點

## Code
```c
struct ListNode *getIntersectionNode(struct ListNode *ha, struct ListNode *hb)
{
	if (ha == NULL || hb == NULL)
		return NULL;
	int len1 = getLength(ha), len2 = getLength(hb);
	struct ListNode *p, *q;
	int diff;
	if (len1 >= len2) {
		p = ha;
		q = hb;
		diff = len1 - len2;
	} else {
		p = hb;
		q = ha;
		diff = len2 - len1;
	}
	while (diff--)
		p = p -> next;
	while (p && q) {
		if (p == q)
			return p;
		p = p->next;
		q = q->next;
	}
	return NULL;
}
```
