#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <climits>
using namespace std;
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x): val(x), next(nullptr) {}
};
class Solution
{
public:
    /**
     * [] [[]]
     */
    ListNode* mergeKLists(vector<ListNode*>& lists)
    {
        return mergeKLists_withMerge(lists);
        //return mergeKLists_withHeap(lists);
    }
private:
    ListNode* mergeKLists_withHeap(vector<ListNode*>& lists)
    {
        auto it_begin = begin(lists);
        auto it_end = end(lists);
        auto isNull = [](ListNode * p) {
            return p == nullptr;
        };
        auto minHeapPred = [](ListNode * t1, ListNode * t2) {
            return t1->val > t2->val;
        };
        it_end = remove_if(it_begin, it_end, isNull);

        if (it_begin == it_end) {
            return nullptr;
        }

        make_heap(it_begin, it_end, minHeapPred);
        ListNode* head = *it_begin;
        ListNode* p = head;

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
    ListNode* mergeTwoList(ListNode* list1, ListNode* list2)
    {
        ListNode* p = list1, *q = list2;
        ListNode* head;

        if (p == nullptr) {
            return q;
        }

        if (q == nullptr) {
            return p;
        }

        if (p->val < q->val) {
            head = p;
            p = p->next;
        } else {
            head = q;
            q = q->next;
        }

        ListNode* t = head;

        while (p && q) {
            if (p->val < q->val) {
                t->next = p;
                p = p->next;
            } else {
                t->next = q;
                q = q->next;
            }

            t = t->next;
        }

        if (p) {
            t->next = p;
        }

        if (q) {
            t->next = q;
        }

        return head;
    }
    ListNode* mergeKLists_withMerge(vector<ListNode*>& lists)
    {
        if (lists.size() == 0) {
            return nullptr;
        }

        return mergeKLists_withMerge(lists, 0, lists.size() - 1);
    }
    ListNode* mergeKLists_withMerge(vector<ListNode*>& lists, int s, int t)
    {
        if (s == t) {
            return lists[s];
        }

        if (t - s == 1) {
            return mergeTwoList(lists[s], lists[t]);
        }

        int mid = s + ((t - s) >> 1);
        ListNode* left = mergeKLists_withMerge(lists, s, mid);
        ListNode* right = mergeKLists_withMerge(lists, mid + 1, t);
        return mergeTwoList(left, right);
    }
    ListNode* mergeKLists_withMerge2(vector<ListNode*>& lists)
    {
        if (lists.empty()) {
            return nullptr;
        }

        while (lists.size() > 1) {
            lists.push_back(mergeTwoList(lists[0], lists[1]));
            lists.erase(lists.begin());
            lists.erase(lists.begin());
        }

        return lists.front();
    }
};
int getLength(ListNode* head)
{
    int len = 0;
    ListNode* p = head;

    while (p) {
        ++len;
        p = p->next;
    }

    return len;
}
void print(ListNode* head)
{
    if (head == nullptr) {
        printf("NULL\n");
        return;
    }

    struct ListNode* p = head;

    while (p) {
        printf("%d ", p->val);
        p = p->next;
    }

    printf("\n");
}
ListNode* mk_list(ListNode** ha, int a[], int n)
{
    if (n < 1) {
        return nullptr;
    }

    ListNode* p = new ListNode(a[0]);
    *ha = p;

    for (int i = 1; i < n; ++i) {
        ListNode* q = new ListNode(a[i]);
        p->next = q;
        p = q;
    }

    return p;
}
ListNode* mk_list(int a[], int n)
{
    if (n < 1) {
        return nullptr;
    }

    ListNode* head = new ListNode(a[0]);
    ListNode* p = head;

    for (int i = 1; i < n; ++i) {
        ListNode* q = new ListNode(a[i]);
        p->next = q;
        p = q;
    }

    return head;
}
void free_list(struct ListNode* head)
{
    struct ListNode* p = head;

    while (p) {
        struct ListNode* q = p->next;
        delete p;
        p = q;
    }
}
int main(int argc, char** argv)
{
    Solution solution;
    int a[] = { -10, -9, -9, -3, -1, -1, 0};
    int b[] = { -9, -6, -5, -4, -2, 2, 3};
    int c[] = { -3, -3, -2, -1, 0};
    //vector<ListNode *> lists = {mk_list(a, 1), mk_list(b, 1), mk_list(c, 1)};
    //vector<ListNode *> lists;
    //vector<ListNode *> lists = {nullptr, nullptr};
    //vector<ListNode *> lists = {nullptr, mk_list(a, 1)};
    //vector<ListNode *> lists = {nullptr};
    vector<ListNode*> lists = {mk_list(a, 7), new ListNode(-5), new ListNode(4), new ListNode(-9), nullptr, mk_list(b, 7), mk_list(c, 5)};
    //vector<ListNode *> lists = {mk_list(a, 7), new ListNode(-5)};
    ListNode* head = solution.mergeKLists(lists);
    print(head);
    return 0;
}
