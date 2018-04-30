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
#include"LinkList.h"
void ListOrder(LinkList L)
/*对链表L进行就地排序*/
{
    ListNode* p, *r, *t, *q;

    p = L->next;            /*p是指向待排序的结点的第一个结点*/

    L->next = NULL;         /*初始时，已排序链表为空*/

    while (p != NULL) {     /*如果待排序链表不为空*/
        if (L->next == NULL) {  /*如果是第一个结点，则作为第一个结点插入L*/
            L->next = p;
            r = p->next;
            p->next = NULL;
            p = r;
        }

        else {                  /*p指向待排序的结点，在L指向的已经排好序的链表中查找插入位置*/
            r = L;
            q = L->next;

            while (q != NULL && p->data > q->data) { /*在q指向的有序表中寻找插入位置*/
                r = q;
                q = q->next;
            }

            t = p->next;        /*t指向p的下一个结点，保存待排序的指针位置*/
            p->next = r->next;  /*将结点*p插入到结点*r的后面*/
            r->next = p;
            p = t;              /*p指向下一个待排序的结点*/
        }
    }

}

void DispList(LinkList L)
/*输出链表L中的元素*/
{
    ListNode* p;
    p = L->next;

    while (p != NULL) {
        printf("%4d", p->data);
        p = p->next;
    }

    printf("\n");
}
void main()
{
    LinkList L;
    ListNode* p;
    int i, m, n;
    DataType a[] = {7, 3, 13, 11, 10, 23, 45, 6};
    n = 8;
    InitList(&L);

    for (i = 0; i < n; i++) {
        InsertList(L, i + 1, a[i]);
    }

    printf("排序前：顺序表L中的元素依次为：");
    DispList(L);
    ListOrder(L);
    printf("排序后：顺序表L中的元素依次为：");
    DispList(L);
}
