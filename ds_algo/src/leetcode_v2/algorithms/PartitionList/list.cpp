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
    ListNode* partition(ListNode* head, int x)
    {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

        ListNode* pre = nullptr;
        ListNode* p = head;
        ListNode* q;

        while (p && p->val >= x) {
            pre = p;
            p = p->next;
        }

        if (p == nullptr) { // p 已经到达结尾节点
            return head;
        }

        q = p->next;

        if (pre) { // 说明第一个元素大于等于x，即若x=3, 6->4->1->8, 需要把1插入最前面，变成1->6->4->8
            pre->next = p->next;
            p->next = head;
            head = p;
        } else {// 说明第一个元素就小于x， 即若x=3， 1->2->8, 则此时pre和q均是nullptr，直接处理下一个节点即可
            pre = p;
        }

        ListNode* smaller = head;
        p = q;

        while (p) {
            q = p->next;

            if (p->val >=
                x) { // 注意后面一个条件，当下一个节点已经正确位置时不需要调整
                pre = p;
            } else {
                if (smaller->next == p) {
                    pre = p;
                    smaller = p;
                } else {
                    pre->next = q;
                    p->next = smaller->next;
                    smaller->next = p;
                    smaller = p;
                }
            }

            p = q;
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
    ListNode* head = NULL;
    int a[20], n, x;

    while (cin >> n >> x) {
        for (int i = 0; i < n; ++i) {
            cin >> a[i];
        }

        mk_list(&head, a, n);
        print(head);
        printf("x = %d\n", x);
        head = solution.partition(head, x);
        printf("result: ");
        print(head);
        cout << endl << endl;
    }

    return 0;
}
