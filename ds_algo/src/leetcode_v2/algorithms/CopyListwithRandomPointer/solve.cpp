#include <iostream>
using namespace std;
struct RandomListNode {
    int label;
    RandomListNode* next, *random;
    RandomListNode(int x): label(x), next(nullptr), random(nullptr) {}
};
void print(RandomListNode* p)
{
    if (p == nullptr) {
        cout << "null" << endl;
        return;
    }

    while (p) {
        cout << p->label << " ";
        p = p->next;
    }

    cout << endl;
}
class Solution
{
public:
    RandomListNode* copyRandomList(RandomListNode* head)
    {
        copyInPlace(head);
        updateRandomPointer(
            head); // 不能和breakdown一起，原因是随机指针可能指向前面的节点
        return breakDown(head);
    }
private:
    RandomListNode* copyInPlace(RandomListNode* head)
    {
        if (head == nullptr) {
            return nullptr;
        }

        RandomListNode* p = head;

        while (p) {
            RandomListNode* t = new RandomListNode(p->label);
            t->next = p->next;
            t->random = p->random;
            p->next = t;
            p = p->next->next;
        }

        return head;
    }
    RandomListNode* updateRandomPointer(RandomListNode* head)
    {
        if (head == nullptr) {
            return nullptr;
        }

        RandomListNode* p = head->next;

        while (p) {
            if (p->random) {
                p->random = p->random->next;
            }

            if (p->next == nullptr) { // 最后一个节点next指针为null
                break;
            } else {
                p = p->next->next;
            }
        }

        return head;
    }
    RandomListNode* breakDown(RandomListNode* head)
    {
        if (head == nullptr) {
            return nullptr;
        }

        RandomListNode* head1 = head, *head2 = head->next;
        RandomListNode* p = head1, *q = head2;

        while (p && q) {
            p->next = p->next->next;

            if (q->next) { // 最后一个节点指向null
                q->next = q->next->next;
            }

            p = p->next;
            q = q->next;
        }

        return head2;
    }
};
RandomListNode* mk_list(RandomListNode** head, int a[], int n)
{
    if (n < 1) {
        *head = nullptr;
    }

    RandomListNode* p = new RandomListNode(a[0]);
    *head = p;

    for (int i = 1; i < n; ++i) {
        RandomListNode* t = new RandomListNode(a[i]);
        p->next = t;
        p = t;
    }

    return *head;
}
int main(int argc, char** argv)
{
    Solution solution;
    RandomListNode* head;
    int a[] = { -1, -1};
    mk_list(&head, a, 2);
    head->random = head->next;
    head->next->random = head;
    RandomListNode* head2 = solution.copyRandomList(head);
    print(head2);
    return 0;
}
