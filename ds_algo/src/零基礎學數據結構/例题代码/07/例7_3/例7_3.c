/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
/*稀疏矩阵类型定义*/

typedef int DataType;
typedef struct OLNode {
    int i, j;
    DataType e;
    struct OLNode* right, *down;
} OLNode, *OLink;
typedef struct {
    OLink* rowhead, *colhead;
    int m, n, len;
} CrossList;

#include"CrossMatrix.h"
void AddMatrix(CrossList A, CrossList B, CrossList* C);
int CompareElement(int a, int b);
void main()
{
    CrossList M, N, Q;
    int row, col;
    DataType value;
    OLink p;
    InitMatrix(&M);     /*初始化稀疏矩阵*/
    CreateMatrix(&M);   /*创建稀疏矩阵*/
    printf("矩阵M：\n");
    PrintMatrix(M);     /*以矩阵的形式输出稀疏矩阵*/

    InitMatrix(&N);     /*初始化稀疏矩阵*/
    CreateMatrix(&N);   /*创建稀疏矩阵*/
    printf("矩阵N：\n");
    PrintMatrix(N); /*以矩阵的形式输出稀疏矩阵*/
    /*两个矩阵的相加*/
    AddMatrix(M, N, &Q);
    printf("两个稀疏矩阵相加结果：M+N=\n");
    PrintMatrix(Q);
    /*在矩阵M中插入一个元素*/
    printf("请输入要插入元素的行号、列号和元素值：");
    scanf("%d,%d,%d", &row, &col, &value);
    p = (OLNode*)malloc(sizeof(OLNode));
    p->i = row;
    p->j = col;
    p->e = value;
    InsertMatrix(&M, p);
    printf("插入元素后，矩阵M：\n");
    PrintMatrix(M);
}

void AddMatrix(CrossList A, CrossList B, CrossList* C)
/*十字链表表示的两个稀疏矩阵相加运算*/
{
    int i;
    OLink pa, pb, pc;

    if (A.m != B.m || A.n != B.n) {
        printf("两个矩阵不是同类型的,不能相加\n");
        exit(-1);
    }

    /*初始化矩阵Q*/
    C->m = A.m;
    C->n = A.n;
    C->len = 0;             /*矩阵C的元素个数的初值为0*/

    /*初始化十字链表*/
    if (!(C->rowhead = (OLink*)malloc(C->m * sizeof(
                                          OLink)))) { /*动态生成行表头数组*/
        exit(-1);
    }

    if (!(C->colhead = (OLink*)malloc(C->n * sizeof(
                                          OLink)))) { /*动态生成列表头数组*/
        exit(-1);
    }

    for (i = 0; i < C->m; i++) { /*初始化矩阵C的行表头指针数组，各行链表为空*/
        C->rowhead[i] = NULL;
    }

    for (i = 0; i < C->n; i++) { /*初始化矩阵C的列表头指针数组，各列链表为空*/
        C->colhead[i] = NULL;
    }

    /*将稀疏矩阵按行的顺序相加*/
    for (i = 0; i < A.m; i++) {
        pa = A.rowhead[i];  /*pa指向矩阵A的第i行的第1个结点*/
        pb = B.rowhead[i];  /*pb指向矩阵B的第i行的第1个结点*/

        while (pa && pb) {
            pc = (OLink)malloc(sizeof(OLNode)); /*生成新结点*/

            switch (CompareElement(pa->j, pb->j)) {
            case -1:                /*如果A的列号小于B的列号，将矩阵A的当前元素值插入C*/
                *pc = *pa;
                InsertMatrix(C, pc);
                pa = pa->right;
                break;

            case  0:                /*如果矩阵A和B的列号相等，元素值相加*/
                *pc = *pa;
                pc->e += pb->e;

                if (pc->e != 0) {   /*如果和为非零，则将结点插入到C中*/
                    InsertMatrix(C, pc);
                } else {
                    free(pc);
                }

                pa = pa->right;
                pb = pb->right;
                break;

            case  1:                /*如果A的列号大于B的列号，将矩阵B的当前元素值插入C中*/
                *pc = *pb;
                InsertMatrix(C, pc);
                pb = pb->right;
            }
        }

        while (pa) {                /*如果矩阵A还有未处理完的非零元素，则将剩余元素插入C中*/
            pc = (OLink)malloc(sizeof(OLNode));
            *pc = *pa;
            InsertMatrix(C, pc);
            pa = pa->right;
        }

        while (pb) {                /*如果矩阵B还有未处理完的非零元素，则将剩余元素插入C中*/
            pc = (OLink)malloc(sizeof(OLNode));
            *pc = *pb;
            InsertMatrix(C, pc);
            pb = pb->right;
        }
    }

    if (C->len == 0) {              /*矩阵C的非零元素个数为零，则直接消耗C*/
        DestroyMatrix(C);
    }
}
int CompareElement(int a, int b)
/*比较两个元素值的大小。如果a>b，返回1，a=b，则返回0，a<b，则返回-1*/
{
    if (a < b) {
        return -1;
    }

    if (a == b) {
        return 0;
    }

    return 1;
}
