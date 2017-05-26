## Reorder List

Given a singly linked list L: `L0→L1→…→Ln-1→Ln`,
reorder it to: `L0→Ln→L1→Ln-1→L2→Ln-2→…`

You must do this in-place without altering the nodes' values.

For example,
Given {1,2,3,4}, reorder it to {1,4,2,3}.

## Solution

首先把鏈表分成左右兩半，若節點個數是奇數，則左邊節點個數比右邊節點個數多一個節點.

使用快慢指針法，找到鏈表的中間節點

```cpp
ListNode *getMidNode(ListNode *head) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *slow = head, *fast = head;
	while (fast != nullptr) {
		slow = slow->next;
		fast = fast->next;
		if (fast)
			fast = fast->next;
	}
	return slow;
}
```

然後把右半部分逆轉,實現逆轉鏈表方法:

```cpp
ListNode *reverse(ListNode *head) {
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

最後把左右兩半指針間隔插入歸併即可:

```cpp
void merge(ListNode *left, ListNode *right) {
	ListNode *p = left, *q = right;
	while (p && q) {
		ListNode *nextLeft = p->next;
		ListNode *nextRight = q->next;
		p->next = q;
		q->next = nextLeft;
		p = nextLeft;
		q = nextRight;
	}
	if (p)
		p->next = nullptr;
}
```

整個代碼流程為:

```cpp
void reorderList(ListNode *head) {
	ListNode *mid = getMidNode(head);
	ListNode *left = head;
	ListNode *right = reverse(mid); 
	merge(left, right);
}
```
