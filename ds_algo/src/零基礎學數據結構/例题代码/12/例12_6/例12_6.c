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
void SelectSort(LinkList L);
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
    DataType a[] = {45, 67, 21, 98, 12, 39, 81, 53};
    InitList(&L);
    CreateList(L, a, n);
    printf("排序前：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
    SelectSort(L);
    printf("排序后：\n");

    for (p = L->next; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }

    printf("\n");
}

void SelectSort(LinkList L)
/*用链表实现选择排序。将链表分为两段，p指向应经排序的链表部分，q指向未排序的链表部分*/
{
    ListNode* p, *q, *t, *s;
    p = L;

    while (p->next->next != NULL) {
        for (s = p, q = p->next; q->next != NULL; q = q->next) /*用q指针进行遍历链表*/
            if (q->next->data <
                s->next->data) { /*如果q指针指向的元素值小于s指向的元素值，则s=q*/
                s = q;
            }

        if (s != q) {       /*如果*s不是最后一个结点，则将s指向的结点链接到p指向的链表后面*/
            t = s->next;    /*将结点*t从q指向的链表中取出*/
            s->next = t->next;
            t->next = p->next; /*将结点*t插入到p指向的链表中*/
            p->next = t;
        }

        p = p->next;
    }
}
