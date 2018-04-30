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
void Interction(LinkList A, LinkList B, LinkList C)
/*将表A和B中具有相同的元素插入到C中*/
{
    ListNode* pa, *pb, *pc;

    pa = A->next, pb = B->next;

    while (pa && pb) {  /*如果A和B都不为空*/
        if (pa->data <
            pb->data) { /*如果A中的元素小于B中的元素，则继续将A中后面的元素与B中当前元素比较*/
            pa = pa->next;
        } else if (pa->data >
                   pb->data) { /*如果A中的元素大于B中的元素，则继续将B中后面的元素与A中当前元素比较*/
            pb = pb->next;
        } else { /*如果A和B元素相等，则将该元素插入到C中*/
            pc = (ListNode*)malloc(sizeof(ListNode));
            pc->data = pa->data;
            pc->next = C->next;
            C->next = pc;
            pa = pa->next;
            pb = pb->next;
        }
    }

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
    LinkList A, B, C;
    ListNode* p;
    int i, m, n;
    DataType a[] = {7, 3, 13, 11, 10, 23, 45, 6};
    DataType b[] = {6, 11, 7, 2, 8, 13};
    n = 8, m = 6;
    InitList(&A);
    InitList(&B);
    InitList(&C);

    for (i = 0; i < n; i++) {
        InsertList(A, i + 1, a[i]);
    }

    for (i = 0; i < m; i++) {
        InsertList(B, i + 1, b[i]);
    }

    printf("顺序表A的元素依次为：");
    DispList(A);
    printf("顺序表B的元素依次为：");
    DispList(B);
    InsertSort(A);
    InsertSort(B);
    printf("顺序表A经过排序后中的元素依次为：");
    DispList(A);
    printf("顺序表B经过排序后中的元素依次为：");
    DispList(B);
    Interction(A, B, C);
    printf("A和B中交集的元素依次为：");
    DispList(C);
}
