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
    ListNode* swapPairs(ListNode* head)
    {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

        ListNode* p = head;
        head = p->next;
        ListNode* pre = nullptr;

        while (p && p->next) {
            ListNode* q = p->next;
            p->next = q->next;
            q->next = p;

            if (pre) {
                pre->next = q;
            }

            pre = p;
            p = p->next;
        }

        return head;
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
    struct ListNode* head = NULL;
    int a[] = {1, 2};
    mk_list(&head, a, 2);
    head = solution.swapPairs(head);
    print(head);
    return 0;
}
