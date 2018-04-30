/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
/*宏定义和单链表类型定义*/
#define ListSize 100
typedef int DataType;
typedef struct Node {
    DataType data;
    struct Node* next;
} ListNode, *LinkList;
/*函数声明*/
LinkList CreateCycList(int n);/*创建一个长度为n的循环单链表的函数声明*/
void Josephus(LinkList head, int n, int m,
              int k); /*在长度为n的循环单链表中，报数为编号为m的出列*/
void DisplayCycList(LinkList head);/*输出循环单链表*/
void main()
{

    LinkList h;
    int n, k, m;
    printf("输入环中人的个数n=");
    scanf("%d", &n);
    printf("输入开始报数的序号k=");
    scanf("%d", &k);
    printf("报数为m的人出列m=");
    scanf("%d", &m);
    h = CreateCycList(n);
    Josephus(h, n, m, k);
}
void Josephus(LinkList head, int n, int m, int k)
/*在长度为n的循环单链表中，从第k个人开始报数，数到m的人出列*/
{
    ListNode* p, *q;
    int i;
    p = head;

    for (i = 1; i < k; i++) { /*从第k个人开始报数*/
        q = p;
        p = p->next;
    }

    while (p->next != p) {
        for (i = 1; i < m; i++) { /*数到m的人出列*/
            q = p;
            p = p->next;
        }

        q->next = p->next;  /*将p指向的结点删除，即报数为m的人出列*/
        printf("%4d", p->data);
        free(p);
        p = q->next;        /*p指向下一个结点，重新开始报数*/
    }

    printf("%4d\n", p->data);
}
LinkList CreateCycList(int n)
/*宏定义和单链表类型定义*/
{
    LinkList head = NULL;
    ListNode* s, *r;
    int i;

    for (i = 1; i <= n; i++) {
        s = (ListNode*)malloc(sizeof(ListNode));
        s->data = i;
        s->next = NULL;

        if (head == NULL) {
            head = s;
        } else {
            r->next = s;
        }

        r = s;
    }

    r->next = head;
    return head;
}
void DisplayCycList(LinkList head)
/*输出循环链表的每一个元素*/
{
    ListNode* p;
    p = head;

    if (p == NULL) {
        printf("该链表是空表");
        return;
    }

    while (p->next != head) { /*如果不是最后一个结点，输出该结点*/
        printf("%4d", p->data);
        p = p->next;
    }

    printf("%4d\n", p->data); /*输出最后一个结点*/
}
