## Palindrome Linked List

Given a singly linked list, determine if it is a palindrome.

Follow up:
Could you do it in O(n) time and O(1) space?

## Solution

找到中间节点，然后原地逆转右半部分，比如左半部分和右半部分是否相等

首先实现找到中间节点，使用快慢指针:

```cpp
ListNode *getMiddleNode(ListNode *head) {
	if (head == nullptr)
		return nullptr;
	ListNode *slow = head, *fast = head;
	while (fast && fast->next) {
		fast = fast->next->next;
		slow = slow->next;
	}
	return slow;
}
```

逆转链表:

```cpp
ListNode *reverse(ListNode *head) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *prev = nullptr;
	ListNode *p = head;
	while (p) {
		ListNode *q = p->next;
		p->next = prev;
		prev = p;
		p = q;
	}
	return prev;
}
```

最后结果:

```cpp
bool isPalindrome(ListNode *head) {
	/* 空节点和单节点 */
	if (head == nullptr || head->next == nullptr)
		return true;
	ListNode *mid = getMiddleNode(head);
	ListNode *h1 = head;
	ListNode *h2 = reverse(mid->next);
	while(h1 && h2) {
		if (h1->val != h2->val)
			return false;
		h1 = h1->next;
		h2 = h2->next;
	}
	return true;

}
```
