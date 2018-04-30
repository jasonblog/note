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
#include"LinkList.h"                    /*包含单链表实现文件*/
void DelElem(LinkList A, LinkList B);   /*删除A中出现B的元素的函数声明*/
void DelElem2(LinkList A, LinkList B);
void main()
{
    int i;
    DataType a[] = {2, 3, 6, 7, 9, 14, 56, 45, 65, 67};
    DataType b[] = {3, 4, 7, 11, 34, 54, 45, 67};

    LinkList A, B;                  /*声明单链表A和B*/
    ListNode* p;
    InitList(&A);                   /*初始化单链表A*/
    InitList(&B);                   /*初始化单链表B*/

    for (i = 1; i <= sizeof(a) / sizeof(a[0]);
         i++) { /*将数组a中元素插入到单链表A中*/
        if (InsertList(A, i, a[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    for (i = 1; i <= sizeof(b) / sizeof(b[0]); i++) { /*将数组b中元素插入单链表B中*/
        if (InsertList(B, i, b[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    printf("单链表A中的元素有%d个：\n", ListLength(A));

    for (i = 1; i <= ListLength(A); i++) { /*输出单链表A中的每个元素*/
        p = Get(A, i);              /*返回单链表A中的每个结点的指针*/

        if (p) {
            printf("%4d", p->data);    /*输出单链表A中的每个元素*/
        }
    }

    printf("\n");
    printf("单链表B中的元素有%d个：\n", ListLength(B));

    for (i = 1; i <= ListLength(B); i++) {
        p = Get(B, i);              /*返回单链表B中的每个每个结点的指针*/

        if (p) {
            printf("%4d", p->data);    /*输出单链表B中的每个元素*/
        }
    }

    printf("\n");
    DelElem2(A, B);                 /*将在单链表A中出现的B的元素删除，即A-B*/
    printf("将在A中出现B的元素删除后(A-B)，现在A中的元素还有%d个：\n",
           ListLength(A));

    for (i = 1; i <= ListLength(A); i++) {
        p = Get(A, i);              /*返回单链表A中每个结点的指针*/

        if (p) {
            printf("%4d", p->data);    /*显示输出删除后A中所有元素*/
        }
    }

    printf("\n");

}
void DelElem(LinkList A, LinkList B)
/*删除A中出现B的元素的函数实现*/
{
    int i, pos;
    DataType e;
    ListNode* p;

    /*在单链表B中，取出每个元素与单链表A中的元素比较，如果相等则删除A中元素对应的结点*/
    for (i = 1; i <= ListLength(B); i++) {
        p = Get(B, i);              /*取出B中的每个结点，将指针返回给p*/

        if (p) {
            pos = LocatePos(A, p->data); /*比较B中的元素是否与A中的元素相等*/

            if (pos > 0) {
                DeleteList(A, pos, &e);    /*如果相等，将其从A中删除*/
            }
        }
    }
}

void DelElem2(LinkList A, LinkList B)
/*删除A中出现B的元素的函数实现*/
{
    ListNode* pre, *p, *r, *q;
    pre = A;
    p = A->next;

    /*在单链表B中，取出每个元素与单链表A中的元素比较，如果相等则删除A中元素对应的结点*/
    while (p != NULL) {
        q = B->next;

        while (q != NULL && q->data != p->data) {
            q = q->next;
        }

        if (q != NULL) {
            r = p;
            pre->next = p->next;
            p = r->next;
            free(r);
        } else {
            pre = p;
            p = p->next;
        }
    }
}
