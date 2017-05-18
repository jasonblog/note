## Remove Nth Node From End of List

Given a linked list, remove the nth node from the end of list and return its head.

For example,
```
   Given linked list: 1->2->3->4->5, and n = 2.

   After removing the second node from the end, the linked list becomes 1->2->3->5.
```
Note:

* Given n will always be valid.
* Try to do this in one pass. 

## Solution

要求只遍历一趟，只需要让两个指针p, q,p先走n步，然后p，q一起走,直到p到达链表结尾，此时q就是需要删除的节点.
