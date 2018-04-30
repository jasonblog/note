#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
/*宏定义和单链表类型定义*/
typedef int DataType;
typedef struct Node { /*链表类型定义*/
    DataType data;
    struct Node* next;
} ListNode, *LinkList;
#include"LinkList.h"                    /*包含单链表实现文件*/
void DisplayList(LinkList L);
void ReverseList(LinkList H);
void main()
{
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    LinkList L;
    InitList(&L);                           /*初始化单链表L*/

    for (i = 1; i <= sizeof(a) / sizeof(a[0]);
         i++) { /*将数组a中元素插入到单链表L中*/
        if (InsertList(L, i, a[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    printf("逆置前链表中的元素：\n");
    DisplayList(L);
    ReverseList(L);
    printf("逆置后链表中的元素：\n");
    DisplayList(L);

}
void ReverseList(LinkList H)
/*逆置链表*/
{
    ListNode* p, *q;
    p = H->next;    /*p指向链表的第一个结点*/
    H->next = NULL;

    while (p) {     /*利用头插法将结点依次插入到链表的头部*/
        q = p->next;
        p->next = H->next;
        H->next = p;
        p = q;
    }
}
void DisplayList(LinkList L)
/*输出链表*/
{
    int i;
    ListNode* p;

    for (i = 1; i <= ListLength(L); i++) { /*输出单链表L中的每个元素*/
        p = Get(L, i);              /*返回单链表L中的每个结点的指针*/

        if (p) {
            printf("%4d", p->data);    /*输出单链表L中的每个元素*/
        }
    }

    printf("\n");
}
