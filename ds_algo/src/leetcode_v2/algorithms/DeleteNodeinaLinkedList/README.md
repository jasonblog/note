## Delete Node in a Linked List 

Write a function to delete a node (except the tail) in a singly linked list, given only access to that node.

Supposed the linked list is `1 -> 2 -> 3 -> 4` and you are given the third node with value `3`, the linked list should become `1 -> 2 -> 4` after calling your function.

## Solution

给定一个单链表节点，删除该节点。

由于无法获得该节点的前驱节点，因此无法更新前驱节点的后继节点。我们可以把该节点的下一个节点的值拷贝到当前节点，转而删除该节点的下一个节点。

该题目也可以描述为: 如何在O(1)的时间删除单链表的某个节点！

```cpp
void deleteNode(ListNode *node) {
	ListNode *p = node->next;
	node->val = p->val;
	node->next = p->next;
	delete p;
}
```
