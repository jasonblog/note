#include <stdio.h>
#include <stdlib.h>
struct ListNode {
    int val;
    struct ListNode* next;

};
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2)
{
    struct ListNode* p = l1, *q = l2, *result = NULL, *k = NULL;

    if (l1 == NULL) {
        return l2;
    }

    if (l2 == NULL) {
        return l1;
    }

    if (p -> val <= q->val) {
        result = p;
        p = p->next;
    } else {
        result = q;
        q = q->next;
    }

    k = result;

    while (p && q) {
        if (p->val <= q->val) {
            k->next = p;
            p = p->next;
        } else {
            k->next = q;
            q = q->next;
        }

        k = k->next;
    }

    while (p) {
        k->next = p;
        p = p->next;
        k = k->next;
    }

    while (q) {
        k->next = q;
        q = q->next;
        k = k->next;
    }

    return result;
}
int main(int argc, char** argv)
{
    struct ListNode* l1 = NULL, *l2 = NULL;
    l1 = malloc(sizeof(*l1));
    l1->val = 2;
    l1->next = malloc(sizeof(*l1));
    l1->next->val = 4;
    l1->next->next = NULL;

    l2 = malloc(sizeof(*l2));
    l2->val = 1;
    l2->next = malloc(sizeof(*l2));
    l2->next->val = 3;
    l2->next->next = NULL;
    struct ListNode* result = mergeTwoLists(l1, l2);

    while (result) {
        printf("%d ", result->val);
        result = result->next;
    }

    return 0;
}
