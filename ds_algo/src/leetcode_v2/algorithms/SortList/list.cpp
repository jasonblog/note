#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
#include <stdlib.h>
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x): val(x), next(nullptr) {}
};
class Solution
{
public:
    ListNode* sortList(ListNode* head)
    {
        return mergeSort(head);
    }
private:
    ListNode* mergeSort(ListNode* head)
    {
        if (!head || !head->next) {
            return head;
        }

        ListNode* slow = head, *fast = head,
                  *prev = head; // 至少2个节点，因此while至少执行一次

        while (fast && fast->next) {
            prev = slow;
            slow = slow->next;
            fast = fast->next->next;
        }

        prev->next = nullptr;
        ListNode* left = mergeSort(head);
        ListNode* right = mergeSort(slow);
        ListNode* merged = merge(left, right);
        return merged;
    }
    ListNode* merge(ListNode* l1, ListNode* l2)
    {
        if (l1 == nullptr) {
            return l2;
        }

        if (l2 == nullptr) {
            return l1;
        }

        if (l1->val <= l2->val) {
            l1->next = merge(l1->next, l2);
            return l1;
        } else {
            l2->next = merge(l1, l2->next);
            return l2;
        }
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
    int a[20], n;
    ListNode* head;

    while (cin >> n) {
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
        }

        mk_list(&head, a, n);
        print(head);
        head = solution.sortList(head);
        print(head);
        cout << endl << endl;
    }

    return 0;
}
