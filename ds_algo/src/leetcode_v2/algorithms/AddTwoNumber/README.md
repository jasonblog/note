# Add Two Number

## Problem
You are given two linked lists representing two non-negative numbers. The digits are stored in reverse order and each of their nodes contain a single digit. Add the two numbers and return it as a linked list.

Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
Output: 7 -> 0 -> 8

## Solution

直接掃描兩個鏈表，和相加，如果大於9，則需要進位，傳遞到下一個節點。

需要注意的問題：

* 注意當兩個鏈表長度不一樣的情況，若前面沒有進位，直接拷貝長的節點，否則需要傳遞進位。
* 注意到最後一個節點時，也有可能出現進位情況

```c
ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {
	ListNode *p = l1, *q = l2, *result = NULL, *last;
	int len1 = 0, len2 = 0;
	while (p && q) {
		len1++, len2++;
		p = p->next;
		q = q->next;
	}
	while(p) {
		len1++;
		p = p->next;
	}
	while (q) {
		len2++;
		q = q->next;
	}
	if (len1 >= len2) {
		p = l1;
		q = l2;
		result = l1;
	}
	else {
		p = l2;
		q = l1;
		result = l2;
	}
	bool carry = false;
	while (p && q) {
		p -> val = (p -> val + q -> val);
		if (carry)
			p -> val++;
		if (p->val >= 10) {
			p->val -= 10;
			carry = true;
		} else {
			carry = false;
		}
		last = p;
		p = p->next;
		q = q->next;
	}
	while (p && carry) {
		if (++(p->val) >= 10) {
			p->val -= 10;
		} else
			carry = false;
		last = p;
		p = p->next;
	}
	if (carry)
		last->next = new ListNode(1);
	return result;
}
```
