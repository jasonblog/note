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
    bool isPalindrome(ListNode* head)
    {
        /* 空节点和单节点 */
        if (head == nullptr || head->next == nullptr) {
            return true;
        }

        ListNode* mid = getMiddleNode(head);
        ListNode* h1 = head;
        ListNode* h2 = reverse(mid->next);

        while (h1 && h2) {
            if (h1->val != h2->val) {
                return false;
            }

            h1 = h1->next;
            h2 = h2->next;
        }

        return true;

    }
private:
    ListNode* getMiddleNode(ListNode* head)
    {
        if (head == nullptr) {
            return nullptr;
        }

        ListNode* slow = head, *fast = head;

        while (fast && fast->next) {
            fast = fast->next->next;
            slow = slow->next;
        }

        return slow;
    }
    ListNode* reverse(ListNode* head)
    {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

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
    int getLength(ListNode* head)
    {
        int size = 0;
        ListNode* p = head;

        while (p) {
            size++;
            p = p->next;
        }

        return size;
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

    return p;
}
ListNode* mk_list(const vector<int>& v)
{
    int n = v.size();

    if (n < 1) {
        return nullptr;
    }

    ListNode* head = new ListNode(v[0]);
    ListNode* p = head;

    for (int i = 1; i < n; ++i) {
        ListNode* q = new ListNode(v[i]);
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
    struct ListNode* head = mk_list( {
                                         1, 2, 2, 1
                                     });
    cout << solution.isPalindrome(head) << endl;
    return 0;
}
