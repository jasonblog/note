## Sort List

Sort a linked list in O(n log n) time using constant space complexity.

## Solution

使用歸併排序。

首先需要把鏈表分成左右兩部分，使用快慢指針法找到中間節點

```cpp
ListNode *slow = head;
ListNode *fast = head,
ListNode *prev = head; // 假設至少2個節點，因此while至少執行一次
while (fast && fast->next) {
	prev = slow;
	slow = slow->next;
	fast = fast->next->next;
}
prev->next = nullptr; // 斷開左右部分
```

此時`left = head, right = slow`:

```cpp
ListNode *left = head;
ListNode *right = slow;
```

分別調用mergeSort對左右部分排序:

```cpp
left = mergeSort(left);
right = mergeSort(right);
```

然後合併左右兩個部分即可:

```cpp
ListNode *merged = merge(left, right);
```

完整代碼:

```cpp
ListNode *mergeSort(ListNode *head) {
		if (!head || !head->next) // 少於2個節點直接返回
			return head;
		ListNode *slow = head;
		ListNode *fast = head;
		ListNode *prev = head; // 至少2個節點，因此while至少執行一次
		while (fast && fast->next) {
			prev = slow;
			slow = slow->next;
			fast = fast->next->next;
		}
		prev->next = nullptr;// 斷開左右兩部分
		ListNode *left = mergeSort(head);
		ListNode *right = mergeSort(slow);
		ListNode *merged = merge(left, right);
		return merged;
	}
```


合併兩個鏈表可以使用迭代法[Merge Two Sorted Lists](../MergeTwoSortedLists)，也可以使用遞歸法，下面是遞歸實現方法:

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

## 擴展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合併兩個鏈表
* [Merge k Sorted Lists](../MergekSortedLists): 合併K個鏈表
* [Merge Sorted Array](../MergeSortedArray): 合併兩個數組
* [Sort List](../SortList): 歸併排序兩個鏈表
