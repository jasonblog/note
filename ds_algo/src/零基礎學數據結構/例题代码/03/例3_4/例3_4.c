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
void MergeList(LinkList A, LinkList B,
               LinkList* C); /*将单链表A和B的元素合并到C中的函数声明*/

void main()
{
    int i;
    DataType a[] = {6, 7, 9, 14, 37, 45, 65, 67};
    DataType b[] = {3, 7, 11, 34, 45, 89};

    LinkList A, B, C;               /*声明单链表A和B*/
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
    MergeList(A, B, &C);                /*将单链表A和B中的元素合并到C中*/
    printf("将单链表A和B的元素合并到C中后，C中的元素有%d个：\n", ListLength(C));

    for (i = 1; i <= ListLength(C); i++) {
        p = Get(C, i);              /*返回单链表C中每个结点的指针*/

        if (p) {
            printf("%4d", p->data);    /*显示输出C中所有元素*/
        }
    }

    printf("\n");

}
void MergeList(LinkList A, LinkList B, LinkList* C)
/*单链表A和B中的元素非递减排列，将单链表A和B中的元素合并到C中，C中的元素仍按照非递减排列*/
{
    ListNode* pa, *pb, *pc; /*定义指向单链表A，B,C的指针*/
    pa = A->next;
    pb = B->next;
    *C = A;             /*将单链表A的头结点作为C的头结点*/
    (*C)->next = NULL;
    pc = *C;

    /*依次将链表A和B中较小的元素存入链表C中*/
    while (pa && pb) {
        if (pa->data <= pb->data) {
            pc->next =
                pa;  /*如果A中的元素小于或等于B中的元素，将A中的元素的结点作为C的结点*/
            pc = pa;
            pa = pa->next;
        } else {
            pc->next = pb;  /*如果A中的元素大于B中的元素，将B中的元素的结点作为C的结点*/
            pc = pb;
            pb = pb->next;
        }
    }

    pc->next = pa ? pa : pb; /*将剩余的结点插入C中*/
    free(B);                /*释放B的头结点*/
}

