#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct ListNode {
    int val;
    struct ListNode* next;
};
struct ListNode* removeElements(struct ListNode* head, int val)
{
    struct ListNode* p = head;

    while (p && p->val == val) {
        struct ListNode* q = p->next;
        free(p);
        p = q;
        head = q;
    }

    while (p) {
        struct ListNode* q = p->next;

        if (q && q->val == val) {
            p->next = q->next;
            free(q);
        } else {
            p = p->next;
        }
    }

    return head;
}
void print(struct ListNode* head)
{
    struct ListNode* p = head;

    if (p == NULL) {
        printf("NULL\n");
        return;
    }

    while (p) {
        printf("%d ", p->val);
        p = p->next;
    }

    printf("\n");
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
    struct ListNode* head;
    int a[] = {1, 2, 2, 1};
    mk_list(&head, a, 4);
    print(head);
    head = removeElements(head, 2);
    print(head);
    return 0;
}
