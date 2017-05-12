## Merge k Sorted Lists

Merge k sorted linked lists and return it as one sorted list. Analyze and describe its complexity.

## Solution 1 

直接从lists中找最小的节点，插入到归并的链表中。

```cpp
class Solution {
	public:
		/**
		 * [] [[]]
		 */
		ListNode *mergeKLists(vector<ListNode*> &lists) {
			int i = getMin(lists);
			if (i < 0)
				return nullptr;
			ListNode *head = lists[i];
			lists[i] = lists[i]->next;
			ListNode *p = head;
			while ((i = getMin(lists)) != -1) {
				p->next = lists[i];
				lists[i] = lists[i]->next;
				p = p->next;
			}
			p->next = nullptr;
			return head;
		}
	private:
		int getMin(vector<ListNode *> &lists) {
			ListNode *minNode = nullptr;
			int pos = -1;
			bool updated = false;
			int i;
			for (i = 0; i < lists.size(); ++i) {
				if (cmp(minNode, lists[i]) > 0) {
					minNode = lists[i];
					pos = i;
					updated = true;
				}
			}
			if (updated)
				return pos;
			else
				return -1;
		}
		int cmp(ListNode *p, ListNode *q)
		{
			if (p == nullptr && q == nullptr)
				return 0;
			if (p == nullptr)
				return 1;
			if (q == nullptr)
				return -1;
			return p->val - q->val;
		}
};
```

这样的瓶颈是每查找一次最小节点都要遍历一遍，复杂度O(n)。总的复杂度为O(n<sup>2</sup>). 

## Solution 2

上一个方案主要的问题是要每次找最小值，我们可以使用堆存储最小值，先堆化整个列表，然后`pop`，pop的值就是当前最小值节点p，
然后让`p = p->next`, 此时如果p不为null，则在push进去，重新堆化，直到只有一个节点即可.

```cpp
ListNode *mergeKLists(vector<ListNode *> &lists) {
	auto it_begin = begin(lists);
	auto it_end = end(lists);
	auto isNull = [](ListNode *p){return p == nullptr;};
	auto minHeapPred = [](ListNode *t1, ListNode *t2){return t1->val > t2->val;};
	it_end = remove_if(it_begin, it_end, isNull);
	if (it_begin == it_end)
		return nullptr;
	make_heap(it_begin, it_end, minHeapPred);
	ListNode *head = *it_begin;
	ListNode *p = head;
	while (distance(it_begin, it_end) > 1) {
		pop_heap(it_begin, it_end, minHeapPred);
		--it_end;
		*it_end = (*it_end)->next;
		if (*it_end) {
			++it_end;
			push_heap(it_begin, it_end, minHeapPred);
		}
		p->next = *it_begin;
		p = p->next;
	}
	return head;
}
```

## Solution 3 迭代法

每次从lists中取出两个链表归并，然后把归并的结果压入lists中，直到只有一个链表为止.

```cpp
ListNode *mergeKLists_withMerge2(vector<ListNode *> &lists)
{
	if (lists.empty())
		return nullptr;
	while (lists.size() > 1) {
		lists.push_back(mergeTwoList(lists[0], lists[1]));
		lists.erase(lists.begin());
		lists.erase(lists.begin());
	}
	return lists.front();
}
```

## Solution 4 分治法

先归并左半部分，然后归并右半部分，最后再归并左右即可

```cpp
ListNode *mergeKLists_withMerge(vector<ListNode *> &lists, int s, int t)
	{
		if (s == t)
			return lists[s];
		if (t - s == 1) {
			return mergeTwoList(lists[s], lists[t]);
		}
		int mid = s + ((t - s) >> 1);
		ListNode *left = mergeKLists_withMerge(lists, s, mid);
		ListNode *right = mergeKLists_withMerge(lists, mid + 1, t);
		return mergeTwoList(left, right);
	}
```

## 扩展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合并两个链表
* [Merge k Sorted Lists](../MergekSortedLists): 合并K个链表
* [Merge Sorted Array](../MergeSortedArray): 合并两个数组
* [Sort List](../SortList): 归并排序两个链表
