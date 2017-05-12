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
        int i = getMin(lists);

        if (i < 0) {
            return nullptr;
        }

        ListNode* head = lists[i];
        lists[i] = lists[i]->next;
        ListNode* p = head;

        while ((i = getMin(lists)) != -1) {
            p->next = lists[i];
            lists[i] = lists[i]->next;
            p = p->next;
        }

        p->next = nullptr;
        return head;
    }
private:
    int getMin(vector<ListNode*>& lists)
    {
        ListNode* minNode = nullptr;
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

        if (updated) {
            return pos;
        } else {
            return -1;
        }
    }
    int cmp(ListNode* p, ListNode* q)
    {
        if (p == nullptr && q == nullptr) {
            return 0;
        }

        if (p == nullptr) {
            return 1;
        }

        if (q == nullptr) {
            return -1;
        }

        return p->val - q->val;
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
    int a[] = {1, 10};
    int b[] = {2, 11};
    int c[] = {0, 12};
    vector<ListNode*> lists = {mk_list(a, 2), mk_list(b, 2), mk_list(c, 2)};
    //vector<ListNode *> lists;
    //vector<ListNode *> lists = {nullptr, nullptr};
    ListNode* head = solution.mergeKLists(lists);
    print(head);
    return 0;
}
