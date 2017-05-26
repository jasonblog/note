## Delete Node in a Linked List 

Write a function to delete a node (except the tail) in a singly linked list, given only access to that node.

Supposed the linked list is `1 -> 2 -> 3 -> 4` and you are given the third node with value `3`, the linked list should become `1 -> 2 -> 4` after calling your function.

## Solution

給定一個單鏈表節點，刪除該節點。

由於無法獲得該節點的前驅節點，因此無法更新前驅節點的後繼節點。我們可以把該節點的下一個節點的值拷貝到當前節點，轉而刪除該節點的下一個節點。

該題目也可以描述為: 如何在O(1)的時間刪除單鏈表的某個節點！

```cpp
void deleteNode(ListNode *node) {
	ListNode *p = node->next;
	node->val = p->val;
	node->next = p->next;
	delete p;
}
```
