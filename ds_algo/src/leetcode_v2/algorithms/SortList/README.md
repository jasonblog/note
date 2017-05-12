## Sort List

Sort a linked list in O(n log n) time using constant space complexity.

## Solution

使用归并排序。

首先需要把链表分成左右两部分，使用快慢指针法找到中间节点

```cpp
ListNode *slow = head;
ListNode *fast = head,
ListNode *prev = head; // 假设至少2个节点，因此while至少执行一次
while (fast && fast->next) {
	prev = slow;
	slow = slow->next;
	fast = fast->next->next;
}
prev->next = nullptr; // 断开左右部分
```

此时`left = head, right = slow`:

```cpp
ListNode *left = head;
ListNode *right = slow;
```

分别调用mergeSort对左右部分排序:

```cpp
left = mergeSort(left);
right = mergeSort(right);
```

然后合并左右两个部分即可:

```cpp
ListNode *merged = merge(left, right);
```

完整代码:

```cpp
ListNode *mergeSort(ListNode *head) {
		if (!head || !head->next) // 少于2个节点直接返回
			return head;
		ListNode *slow = head;
		ListNode *fast = head;
		ListNode *prev = head; // 至少2个节点，因此while至少执行一次
		while (fast && fast->next) {
			prev = slow;
			slow = slow->next;
			fast = fast->next->next;
		}
		prev->next = nullptr;// 断开左右两部分
		ListNode *left = mergeSort(head);
		ListNode *right = mergeSort(slow);
		ListNode *merged = merge(left, right);
		return merged;
	}
```


合并两个链表可以使用迭代法[Merge Two Sorted Lists](../MergeTwoSortedLists)，也可以使用递归法，下面是递归实现方法:

```cpp
ListNode *merge(ListNode *l1, ListNode *l2) {
	if (l1 == nullptr)
		return l2;
	if (l2 == nullptr)
		return l1;
	if (l1->val <= l2->val) {
		l1->next = merge(l1->next, l2);
		return l1;
	} else {
		l2->next = merge(l1, l2->next);
		return l2;
	}
}
```

## 扩展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合并两个链表
* [Merge k Sorted Lists](../MergekSortedLists): 合并K个链表
* [Merge Sorted Array](../MergeSortedArray): 合并两个数组
* [Sort List](../SortList): 归并排序两个链表
