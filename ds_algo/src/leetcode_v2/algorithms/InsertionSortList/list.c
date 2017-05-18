#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct ListNode {
    int val;
    struct ListNode* next;
};
struct ListNode* insertionSortList(struct ListNode* head)
{
    if (head == NULL) {
        return NULL;
    }

    struct ListNode* p = head->next;

    struct ListNode* prev = head;

    while (p) {
        if (p->val >= prev->val) {
            prev = p;
            p = p->next;
            continue;
        }

        if (p->val <= head->val) {
            prev->next = p->next;
            p->next = head;
            head = p;
            p = prev->next;
        } else {
            struct ListNode* t = head;

            while (t->next->val < p->val) {
                t = t->next;
            }

            prev->next = p->next;
            p->next = t->next;
            t->next = p;
            p = prev->next;

        }
    }

    return head;
}
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
void free_list(struct ListNode* head)
{
    struct ListNode* p = head;

    while (p) {
        struct ListNode* q = p->next;
        free(p);
        p = q;
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
    struct ListNode* head = NULL;
    int a[100], n;

    while (scanf("%d", &n) != EOF) {
        free_list(head);

        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        mk_list(&head, a, n);
        head = insertionSortList(head);
        print(head);
    }

    return 0;
}
