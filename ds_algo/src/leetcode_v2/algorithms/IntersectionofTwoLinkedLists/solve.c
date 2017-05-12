#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct ListNode {
    int val;
    struct ListNode* next;
};
int getLength(struct ListNode* head)
{
    int len = 0;
    struct ListNode* p = head;

    while (p) {
        ++len;
        p = p->next;
    }

    return len;
}
struct ListNode* getIntersectionNode(struct ListNode* ha, struct ListNode* hb)
{
    if (ha == NULL || hb == NULL) {
        return NULL;
    }

    int len1 = getLength(ha), len2 = getLength(hb);
    struct ListNode* p, *q;
    int diff;

    if (len1 >= len2) {
        p = ha;
        q = hb;
        diff = len1 - len2;
    } else {
        p = hb;
        q = ha;
        diff = len2 - len1;
    }

    while (diff--) {
        p = p -> next;
    }

    while (p && q) {
        if (p == q) {
            return p;
        }

        p = p->next;
        q = q->next;
    }

    return NULL;
}
void print(struct ListNode* ha, struct ListNode* hb)
{
    struct ListNode* p = getIntersectionNode(ha, hb);

    if (p == NULL) {
        printf("NULL\n");
    } else {
        printf("%d\n", p -> val);
    }
}
struct ListNode* mk_list(struct ListNode** ha, int a[], int n)
{
    struct ListNode* p = malloc(sizeof(*p));
    p->val = a[0];
    p->next = NULL;
    *ha = p;

    for (int i = 1; i < n; ++i) {
        struct ListNode* q = malloc(sizeof(*q));
        q->val = a[i];
        q->next = NULL;
        p->next = q;
        p = q;
    }

    return p;
}
int main(int argc, char** argv)
{
    struct ListNode* ha = NULL, *hb = NULL;
    print(ha, hb);
    int a[] = {1, 2, 3, 4, 5};
    int b[] = {6, 7};
    struct ListNode* last1 = mk_list(&ha, a, 5);
    struct ListNode* last2 = mk_list(&hb, b, 2);
    last2->next = ha;
    print(ha, hb);
    return 0;
}
