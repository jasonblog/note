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
void print(struct ListNode* head)
{
    if (head == NULL) {
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
void free_list(struct ListNode* head)
{
    struct ListNode* p = head;

    while (p) {
        struct ListNode* q = p->next;
        free(p);
        p = q;
    }
}
struct ListNode* deleteDuplicates(struct ListNode* head)
{
    struct ListNode* p = head;

    while (p) {
        struct ListNode* q = p->next;

        if (q && p->val == q->val) {
            p->next = p->next->next;
            free(q);
        } else {
            p = p->next;
        }
    }

    return head;
}
int main(int argc, char** argv)
{
    struct ListNode* head = NULL;
    int a[] = {1, 1, 1, 2, 2, 3, 3, 4};
    mk_list(&head, a, 8);
    print(head);
    deleteDuplicates(head);
    print(head);
    return 0;
}
