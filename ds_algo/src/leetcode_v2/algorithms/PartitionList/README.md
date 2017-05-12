## Partition List

Given a linked list and a value x, partition it such that all nodes less than x come before nodes greater than or equal to x.

You should preserve the original relative order of the nodes in each of the two partitions.

For example,
Given `1->4->3->2->5->2` and `x = 3`,
return `1->2->2->4->3->5`.

## Solution

类似插入排序，假设前面均已经处理好，若当前值小于x，则把x追加到已处理节点的后面

```cpp
ListNode* partition(ListNode* head, int x) {
	if (head == nullptr || head->next == nullptr)
		return head;
	ListNode *pre = nullptr;
	ListNode *p = head;
	ListNode *q;
	while (p && p->val >= x) { // 找到第一个小于x的节点
		pre = p;
		p = p->next;
	}
	if (p == nullptr) // p 已经到达结尾节点,不用更新
		return head;
	q = p->next; // 保存下一个节点
	if (pre) { // 说明p不是第一个节点，需要更新head节点，即若x=3, 6->4->1->8, 需要把1插入最前面，变成1->6->4->8
		pre->next = p->next;
		p->next = head;
		head = p;
	} else {// 说明p就是第一个节点， 即若x=3， 1->2->8, 直接处理下一个节点即可,无需更新head节点
		pre = p;
	}
	ListNode *smaller = head; // smaller 保存已经处理节点的最后一个节点，head已经处理完毕
	p = q; // p从下一个节点开始
	while (p) {
		q = p->next;
		if (p->val >= x) { // 无需处理
			pre = p;
		} else {
			if (smaller->next == p) { // 如果p就是smaller的下一个节点，不需要调整，已经是正确位置
				pre = p;
			} else { // 把p插入到smaller的尾部
				pre->next = q; 
				p->next = smaller->next;
				smaller->next = p;
			}
			smaller = p; // 更新smaller节点为尾部节点
		}
		p = q;
	}
	return head;
}
```
