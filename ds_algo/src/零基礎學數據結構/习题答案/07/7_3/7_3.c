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
void MultMatrix(CrossList A, CrossList B, CrossList* C);
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
    /*两个矩阵的相乘*/
    MultMatrix(M, N, &Q);
    printf("两个稀疏矩阵相乘结果：M+N=\n");
    PrintMatrix(Q);
}

void MultMatrix(CrossList M, CrossList N, CrossList* Q)
/*求稀疏矩阵M和N的的乘积Q=M×N*/
{
    int i, j, e;
    OLink pq, pm, pn;
    InitMatrix(Q);
    Q->m = M.m;
    Q->n = N.n;
    Q->len = 0;
    /*初始化Q的表头指针向量*/
    Q->rowhead = (OLink*)malloc(Q->m * sizeof(OLink)); /*生成行表头指针向量*/

    if (!M.rowhead) {
        exit(-1);
    }

    Q->colhead = (OLink*)malloc(Q->n * sizeof(OLink)); /*生成列表头指针向量*/

    if (!M.colhead) {
        exit(-1);
    }

    for (i = 0; i < Q->m; i++) { /*初始化矩阵Q的行表头指针向量，将各行链表为空*/
        Q->rowhead[i] = NULL;
    }

    for (i = 0; i < Q->n; i++) { /*初始化矩阵Q的列表头指针向量，将各列链表为空*/
        Q->colhead[i] = NULL;
    }

    /*两个矩阵的相乘*/
    for (i = 0; i < Q->m; i++)
        for (j = 0; j < Q->n; j++) {
            pm = M.rowhead[i];
            pn = N.colhead[j];
            e = 0;

            while (pm && pn)
                switch (CompareElement(pn->i, pm->j)) {
                case -1:
                    pn = pn->down; /*列指针后移*/
                    break;

                case 0:
                    e += pm->e * pn->e;
                    pn = pn->down; /*行列指针均后移*/
                    pm = pm->right;
                    break;

                case 1:
                    pm = pm->right; /*行指针后移*/
                }

            if (e) {             /*如果相乘结果不为0，则将该值插入到Q中*/
                pq = (OLink)malloc(sizeof(OLNode));

                if (!pq) {
                    exit(-1);
                }

                pq->i = i;
                pq->j = j;
                pq->e = e;
                InsertMatrix(Q, pq); /*将结点pq按行列值升序插到矩阵Q中*/
            }
        }

    if (Q->len == 0) {       /*如果Q矩阵元素个数为0，则销毁矩阵*/
        DestroyMatrix(Q);
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
