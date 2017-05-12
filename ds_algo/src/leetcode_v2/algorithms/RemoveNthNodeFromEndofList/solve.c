#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct ListNode {
    int val;
    struct ListNode* next;
};
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
    if (head == NULL) {
        return NULL;
    }

    struct ListNode* p = head, *q = head;

    int i = 0;

    for (i = 0; i < n && p; ++i) {
        p = p -> next;
    }

    struct ListNode* t = NULL;

    if (i < n) {
        return head;
    }

    while (p) {
        p = p -> next;
        t = q;
        q = q -> next;
    }

    if (t != NULL) {
        t -> next = q -> next;
        q -> next = NULL;
        free(q);
        q = NULL;
        return head;
    } else {
        head = head -> next;
        free(q);
        q = NULL;
        return head;
    }
}
void init(struct ListNode** head, int a[], int n)
{
    struct ListNode* p = malloc(sizeof(*p));
    p -> val = a[0];
    *head = p;

    for (int i = 1; i < n; ++i) {
        struct ListNode* t = malloc(sizeof(*t));
        t->val = a[i];
        p -> next = t;
        p = t;
    }
}
void print(struct ListNode* head)
{
    struct ListNode* p = head;

    while (p) {
        printf("%d ", p->val);
        p = p->next;
    }

    printf("\n");
}
struct ListNode* rm(struct ListNode* head, int n)
{
    return removeNthFromEnd(head, n);
}
int main(int argc, char** argv)
{
    struct ListNode* head = NULL;
    int a[] = {1, 2, 3, 4, 5};
    init(&head, a, 5);
    print(head);
    head = rm(head, 2);
    print(head);
    head = rm(head, 4);
    print(head);
    head = rm(head, 100);
    print(head);
    return 0;
}
