## Merge k Sorted Lists

Merge k sorted linked lists and return it as one sorted list. Analyze and describe its complexity.

## Solution 1 

直接從lists中找最小的節點，插入到歸併的鏈表中。

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

這樣的瓶頸是每查找一次最小節點都要遍歷一遍，複雜度O(n)。總的複雜度為O(n<sup>2</sup>). 

## Solution 2

上一個方案主要的問題是要每次找最小值，我們可以使用堆存儲最小值，先堆化整個列表，然後`pop`，pop的值就是當前最小值節點p，
然後讓`p = p->next`, 此時如果p不為null，則在push進去，重新堆化，直到只有一個節點即可.

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

每次從lists中取出兩個鏈表歸併，然後把歸併的結果壓入lists中，直到只有一個鏈表為止.

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

先歸併左半部分，然後歸併右半部分，最後再歸併左右即可

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

## 擴展

* [Merge Two Sorted Lists](../MergeTwoSortedLists): 合併兩個鏈表
* [Merge k Sorted Lists](../MergekSortedLists): 合併K個鏈表
* [Merge Sorted Array](../MergeSortedArray): 合併兩個數組
* [Sort List](../SortList): 歸併排序兩個鏈表
