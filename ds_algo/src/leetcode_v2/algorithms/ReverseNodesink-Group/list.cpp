#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
#include <stdlib.h>
#include <cassert>
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x): val(x), next(nullptr) {}
};
class Solution
{
public:
    ListNode* reverseKGroup(ListNode* head, int k)
    {
        if (head == nullptr || k <= 1) {
            return head;
        }

        ListNode* begin = head, *end = nullptr;
        ListNode* prev = nullptr;

        /* First reverse */
        /* 第一次操作，主要需要更新head和prev指针 */
        if (begin) {
            int K = k - 1;
            ListNode* p = begin;

            while (K && p->next) {
                end = p->next;
                p = p->next;
                K--;
            }

            if (K) {
                return head;
            }

            ListNode* next = reverse(begin, end);
            prev = begin;
            head = end;
            begin = next;
        }

        while (begin) {
            int K = k - 1;
            ListNode* p = begin;

            while (K && p->next) {
                end = p->next;
                p = p->next;
                K--;
            }

            if (K) { // 不足k个节点，不需要reverse，但需要更新处理完毕的尾部节点
                prev->next = begin; // 记得最后更新prev指针
                return head;
            }

            ListNode* next = reverse(begin, end);
            prev->next = end;
            prev = begin;
            begin = next;
        }

        return head;
    }
private:
    ListNode* reverse(ListNode* begin, ListNode* end)
    {
        assert(begin && end);
        ListNode* p = begin;
        ListNode* prev = nullptr;
        ListNode* endFlag = end->next;

        while (p !=
               endFlag) { // 条件不能写成 p != end->next, 因为end->next reverse时会修改
            ListNode* q = p->next;
            p->next = prev;
            prev = p;
            p = q;
        }

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
    int a[] = {1, 2, 3, 4, 5, 6};
    mk_list(&head, a, 6);
    print(head);
    head = solution.reverseKGroup(head, 3);
    print(head);
    return 0;
}
