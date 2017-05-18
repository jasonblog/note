#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
struct ListNode {
    int val;
    struct ListNode* next;
};
bool hasCycle(struct ListNode* head)
{
    if (head == NULL) {
        return false;
    }

    struct ListNode* p = head, *q = head->next;;

    while (p && q) {
        if (p == q) {
            return true;
        }

        p = p -> next;
        q = q -> next;

        if (q == NULL) {
            return false;
        }

        q = q -> next;
    }
}
void mk_list(struct ListNode** head, int a[], int n)
{
    struct ListNode* p = malloc(sizeof(*p));
    *head = p;
    p->val = a[0];
    p->next = NULL;

    for (int i = 1; i < n; ++i) {
        struct ListNode* t = malloc(sizeof(*t));
        t->val = a[i];
        t->next = NULL;
        p->next = t;
        p = t;
    }
}
int main(int argc, char** argv)
{
    struct ListNode* head;
    int a[] = {1, 2, 3, 4, 5};
    mk_list(&head, a, 5);
    head->next->next = head;
    printf("%d\n", hasCycle(head));
    return 0;
}
