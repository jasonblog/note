## Palindrome Linked List

Given a singly linked list, determine if it is a palindrome.

Follow up:
Could you do it in O(n) time and O(1) space?

## Solution

找到中間節點，然後原地逆轉右半部分，比如左半部分和右半部分是否相等

首先實現找到中間節點，使用快慢指針:

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

逆轉鏈表:

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

最後結果:

```cpp
bool isPalindrome(ListNode *head) {
	/* 空節點和單節點 */
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
