/*包含头文件*/
#include<stdio.h>
#include<malloc.h>

/*宏定义和单链表类型定义*/
#define ListSize 100
typedef int DataType;
typedef struct Node {
    DataType data;
    struct Node* next;
} ListNode, *LinkList;
/*函数声明*/
LinkList CreateCycList(int n);              /*创建一个循环单链表的函数声明*/
void DisplayCycList();                      /*显示输出循环单链表的函数声明*/
LinkList Link(LinkList head1, LinkList head2); /*将两个链表连接一起的函数声明*/
void main()
{
    LinkList h1, h2;
    int n;
    printf("创建一个循环单链表h1：\n");
    printf("请输入元素个数：\n");
    scanf("%d", &n);
    h1 = CreateCycList(n);
    printf("创建一个循环单链表h2：\n");
    printf("请输入元素个数：\n");
    scanf("%d", &n);
    h2 = CreateCycList(n);
    printf("输出循环单链表h1\n");
    DisplayCycList(h1);
    printf("输出循环单链表h2\n");
    DisplayCycList(h2);
    h1 = Link(h1, h2);
    printf("输出连接后的循环单链表h1+h2\n");
    DisplayCycList(h1);
}
LinkList Link(LinkList head1, LinkList head2)
/*将两个链表head1和head2连接在一起形成一个循环链表*/
{
    ListNode* p, *q;
    p = head1;

    while (p->next != head1) { /*指针p指向链表的最后一个结点*/
        p = p->next;
    }

    q = head2;

    while (q->next != head2) { /*指针q指向链表的最后一个结点*/
        q = q->next;
    }

    p->next = head2;    /*将第一个链表的尾端连接到第二个链表的第一个结点*/
    q->next = head1;    /*将第二个链表的尾端连接到第一个链表的第一个结点*/
    return head1;
}
LinkList CreateCycList(int n)
/*创建一个不带头结点的循环单链表*/
{
    DataType e;
    LinkList head = NULL;
    ListNode* p, *q;
    int i;
    i = 1;
    q = NULL;

    while (i <= n) {
        printf("请输入第%d个元素.", i);
        scanf("%d", &e);

        if (i == 1) {       /*创建第一个结点*/
            head = (LinkList)malloc(sizeof(ListNode));
            head->data = e;
            head->next = NULL;
            q = head;       /*指针q指向链表的最后一个结点*/
        } else {
            p = (ListNode*)malloc(sizeof(ListNode));
            p->data = e;
            p->next = NULL;
            q->next = p;    /*将新结点连接到链表中*/
            q = p;          /*q始终指向最后一个结点*/
        }

        i++;
    }

    if (q != NULL) {
        q->next = head;    /*将最后一个结点的指针指向头指针，使其形成一个循环链表*/
    }

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
