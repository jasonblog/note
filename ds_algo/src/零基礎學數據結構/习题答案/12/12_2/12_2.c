/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
typedef int KeyType;
typedef struct { /*数据元素类型定义*/
    KeyType key;/*关键字*/
} DataType;
typedef struct Node {   /*单链表类型定义*/
    DataType data;
    struct Node* next;
} ListNode, *LinkList;

void SelectSort(LinkList L);
void CreateList(LinkList L, DataType a[], int n);
void BubbleSort(LinkList L);
void CreateList(LinkList L, DataType a[], int n)
/*创建单链表*/
{
    int i;

    for (i = 1; i <= n; i++) {
        InsertList(L, i, a[i - 1]);
    }
}
void InitList(LinkList* head)
/*将单链表初始化为空。动态生成一个头结点，并将头结点的指针域置为空。*/
{
    if ((*head = (LinkList)malloc(sizeof(ListNode))) ==
        NULL) { /*为头结点分配一个存储空间*/
        exit(-1);
    }

    (*head)->next = NULL;           /*将单链表的头结点指针域置为空*/
}
int InsertList(LinkList head, int i, DataType e)
/*在单链表中第i个位置插入一个结点，结点的元素值为e。插入成功返回1，失败返回0*/
{
    ListNode* p,
              *pre;  /*定义指向第i个元素的前驱结点指针pre，指针p指向新生成的结点*/
    int j;
    pre = head;         /*指针p指向头结点*/
    j = 0;

    while (pre->next != NULL &&
           j < i - 1) { /*找到第i－1个结点，即第i个结点的前驱结点*/
        pre = pre->next;
        j++;
    }

    if (!pre) {                 /*如果没找到，说明插入位置错误*/
        printf("插入位置错");
        return 0;
    }

    /*新生成一个结点，并将e赋值给该结点的数据域*/
    if ((p = (ListNode*)malloc(sizeof(ListNode))) == NULL) {
        exit(-1);
    }

    p->data = e;
    /*插入结点操作*/
    p->next = pre->next;
    pre->next = p;
    return 1;
}
void main()
{
    LinkList L, p;
    int n = 8;
    DataType a[] = {45, 67, 21, 98, 12, 39, 81, 53};
    InitList(&L);
    CreateList(L, a, n);
    printf("排序前：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
    BubbleSort(L);
    printf("冒泡排序后：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
}

void BubbleSort(LinkList L)
/*用链表存储结构实现冒泡排序*/
{
    ListNode* p, *q, *s;
    s = NULL;               /*指针s指向已经排好序的第一个结点*/

    while (s != L->next) {  /*如果还有待排序元素，则继续比较*/
        q = L;
        p = q->next;        /*结点*p为待排序链表中的第一个结点*/

        while (p->next != s) { /*如果在该趟排序中，还有元素需要比较*/
            if (p->data.key > p->next->data.key) { /*交换两个结点的位置*/
                q->next = p->next;
                p->next = q->next->next;
                q->next->next = p;
            }

            q = q->next;
            p = q->next;
        }

        s = p;              /*s指向已经排好序的链表的第一个结点*/
    }
}
