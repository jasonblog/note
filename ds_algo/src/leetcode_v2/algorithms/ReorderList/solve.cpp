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
    void reorderList(ListNode* head)
    {
        ListNode* mid = getMidNode(head);
        ListNode* left = head;
        ListNode* right = reverse(mid);
        merge(left, right);
    }
    ListNode* getMidNode(ListNode* head)
    {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

        ListNode* slow = head, *fast = head;

        while (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;

            if (fast) {
                fast = fast->next;
            }
        }

        return slow;
    }
    ListNode* reverse(ListNode* head)
    {
        ListNode* prev = nullptr;
        ListNode* p = head;

        while (p) {
            ListNode* q = p->next;
            p->next = prev;
            prev = p;
            p = q;
        }

        return prev;
    }
    void merge(ListNode* left, ListNode* right)
    {
        ListNode* p = left, *q = right;

        while (p && q) {
            ListNode* nextLeft = p->next;
            ListNode* nextRight = q->next;
            p->next = q;
            q->next = nextLeft;
            p = nextLeft;
            q = nextRight;
        }

        if (p) {
            p->next = nullptr;
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
    struct ListNode* head = NULL;
    int a[20], n;

    while (cin >> n) {
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
        }

        mk_list(&head, a, n);
        cout << "Before: ";
        print(head);
        solution.reorderList(head);
        cout << "After: ";
        print(head);
    }

    return 0;
}
