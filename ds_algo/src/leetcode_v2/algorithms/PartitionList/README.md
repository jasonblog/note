## Partition List

Given a linked list and a value x, partition it such that all nodes less than x come before nodes greater than or equal to x.

You should preserve the original relative order of the nodes in each of the two partitions.

For example,
Given `1->4->3->2->5->2` and `x = 3`,
return `1->2->2->4->3->5`.

## Solution

類似插入排序，假設前面均已經處理好，若當前值小於x，則把x追加到已處理節點的後面

```cpp
ListNode* partition(ListNode* head, int x) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *pre = nullptr;
	ListNode *p = head;
	ListNode *q;
	while (p && p->val >= x) { // 找到第一個小於x的節點
		pre = p;
		p = p->next;
	}
	if (p == nullptr) // p 已經到達結尾節點,不用更新
		return head;
	q = p->next; // 保存下一個節點
	if (pre) { // 說明p不是第一個節點，需要更新head節點，即若x=3, 6->4->1->8, 需要把1插入最前面，變成1->6->4->8
		pre->next = p->next;
		p->next = head;
		head = p;
	} else {// 說明p就是第一個節點， 即若x=3， 1->2->8, 直接處理下一個節點即可,無需更新head節點
		pre = p;
	}
	ListNode *smaller = head; // smaller 保存已經處理節點的最後一個節點，head已經處理完畢
	p = q; // p從下一個節點開始
	while (p) {
		q = p->next;
		if (p->val >= x) { // 無需處理
			pre = p;
		} else {
			if (smaller->next == p) { // 如果p就是smaller的下一個節點，不需要調整，已經是正確位置
				pre = p;
			} else { // 把p插入到smaller的尾部
				pre->next = q; 
				p->next = smaller->next;
				smaller->next = p;
			}
			smaller = p; // 更新smaller節點為尾部節點
		}
		p = q;
	}
	return head;
}
```
