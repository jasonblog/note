/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

typedef int DataType;   /*元素类型定义为整型*/
typedef struct Node {   /*单链表类型定义*/
    DataType data;
    struct Node* next;
} ListNode, *LinkList;
#include"LinkList.h"
void InsertSort(LinkList L);
void CreateList(LinkList L, DataType a[], int n);
void CreateList(LinkList L, DataType a[], int n)
/*创建单链表*/
{
    int i;

    for (i = 1; i <= n; i++) {
        InsertList(L, i, a[i - 1]);
    }
}
void main()
{
    LinkList L, p;
    int n = 8;
    DataType a[] = {87, 34, 22, 93, 102, 56, 39, 21};
    InitList(&L);
    CreateList(L, a, n);
    printf("排序前：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
    InsertSort(L);
    printf("排序后：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
}

void InsertSort(LinkList L)
/*链表的插入排序*/
{
    ListNode* p = L->next, *pre, *q;
    L->next = NULL;         /*初始时，已排序链表为空*/

    while (p != NULL) {     /*p是指向待排序的结点*/
        if (L->next ==
            NULL) { /*如果*p是第一个结点，则插入到L，并令已排序的最后一个结点的指针域为空*/
            L->next = p;
            p = p->next;
            L->next->next = NULL;
        } else {            /*p指向待排序的结点，在L指向的已经排好序的链表中查找插入位置*/
            pre = L;
            q = L->next;

            while (q != NULL && q->data < p->data) { /*在q指向的有序表中寻找插入位置*/
                pre = q;
                q = q->next;
            }

            q = p->next;                    /*q指向p的下一个结点，保存待排序的指针位置*/
            p->next = pre->next;            /*将结点*p插入到结点*pre的后面*/
            pre->next = p;
            p = q;                          /*p指向下一个待排序的结点*/
        }
    }
}
