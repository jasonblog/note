#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct node {
    int value;
    struct node* next;
} list, *List;

/**
 * 寻找环的第一次相遇点，如果没有环返回NULL
 */
List findFirstPoint(List head)
{
    if (head == NULL) {
        return NULL;
    }

    List fast = head, slow = head;

    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;

        if (fast == slow) {
            return fast;
        }
    }

    return NULL;
}
/**
 * 判断链表是否存在环
 */
bool hasCycle(List head)
{
    return findFirstPoint(head) != NULL;
}
/**
 * 若存在环，返回环入口点，否则返回NULL
 */
List detectCycle(List head)
{
    List p = findFirstPoint(head);

    if (p == NULL) {
        return NULL;
    }

    List q = head;

    while (p != q) {
        p = p->next;
        q = q->next;
    }

    return p;
}
/**
 * 如果存在环，返回环的长度，否则返回0
 */
int lengthOfCycle(List head)
{
    List p = findFirstPoint(head);

    if (p == NULL) {
        return 0;
    }

    List slow = p, fast = p;
    int len = 0;

    do {
        len++;
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != slow);

    return len;
}
/**
 * 根据数组构造链表，无环，最后一个节点的next指向NULL，返回最后一个节点
 */
List make_list(List* head, int a[], int n)
{
    if (n < 1) {
        *head = NULL;
        return NULL;
    }

    List p = malloc(sizeof(*p));
    p->next = NULL;
    p->value = a[0];
    *head = p;

    for (int i = 1; i < n; ++i) {
        List t = malloc(sizeof(*t));
        t->value = a[i];
        t->next = NULL;
        p->next = t;
        p = t;
    }

    return p;
}
/**
 * 返回链表头指针
 */
List first(List head)
{
    return head;
}
/**
 * 针对无环链表，返回最后一个节点
 */
List lastWithoutCycle(List head)
{
    List p = head;

    if (p == NULL) {
        return NULL;
    }

    while (p->next) {
        p = p->next;
    }

    return p;
}
/**
 * 返回链表最后一个节点，能够处理有环的情况
 */
List last(List head)
{
    List cycle = detectCycle(head);

    if (cycle == NULL) {
        return lastWithoutCycle(head);
    }

    List p = cycle;

    while (p->next != cycle) {
        p = p->next;
    }

    return p;
}
/**
 * 从链表中查找第一个值等于key的节点
 */
List find(List head, int key)
{
    List p = head;
    List l = last(head);

    while (p) {
        if (p->value == key) {
            return p;
        }

        if (p == l) {
            break;
        }

        p = p->next;
    }

    return NULL;
}
/**
 * 判断该链表中是否存在值为key的节点
 */
bool contains(List head, int key)
{
    return find(head, key) != NULL;
}
int length(List head)
{
    int len = 0;
    List p = head;
    List l = last(head);

    while (p) {
        len++;

        if (p == l) {
            break;
        }

        p = p->next;
    }

    return len;
}

void print_node(List p)
{
    if (p == NULL) {
        printf("null\n");
    } else {
        printf("%d\n", p->value);
    }
}
void print(List p)
{
    while (p) {
        printf("%d ", p->value);
        p = p->next;
    }

    printf("\n");
}
int main(int argc, char** argv)
{
    int a[] = {1, 2, 3, 4, 5};
    List head;
    List l = make_list(&head, a, 5);
    l->next = find(head, 2);
    print_node(find(head, 4));
    printf("%d\n", length(head));
    return 0;
}
