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
    ListNode* reverseList(ListNode* head)
    {
        return reverseIteratively(head);
    }
private:
    ListNode* reverseIteratively(ListNode* head)
    {
        if (!head || !head->next) {
            return head;
        }

        ListNode* prev = nullptr, *p = head;

        while (p) {
            ListNode* q = p->next;
            p->next = prev;
            prev = p;
            p = q;
        }

        return prev;
    }
    ListNode* reverseRecursively(ListNode*& head, ListNode* p)
    {
        if (!p || !p->next) {
            head = p;
            return head;
        }

        reverseRecursively(head, p->next)->next = p;
        p->next = nullptr;
        return p;
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
    int a[] = {1, 2, 3, 4, 5};
    mk_list(&head, a, 5);
    head = solution.reverseList(head);
    print(head);
    return 0;
}
