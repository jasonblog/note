/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
/*稀疏矩阵类型定义*/
#define MaxSize 200
typedef int DataType;
typedef struct {        /*三元组类型定义*/
    int i, j;
    DataType e;
} Triple;
typedef struct {        /*矩阵类型定义*/
    Triple data[MaxSize];
    int rpos[MaxSize];
    int m, n, len;      /*矩阵的行数，列数和非零元素的个数*/
} TriSeqMatrix;
/*函数声明*/
void MultMatrix(TriSeqMatrix A, TriSeqMatrix B, TriSeqMatrix* C);
void PrintMatrix(TriSeqMatrix M);
int CreateMatrix(TriSeqMatrix* M);

void main()
{
    TriSeqMatrix M, N, Q;
    CreateMatrix(&M);
    PrintMatrix(M);
    CreateMatrix(&N);
    PrintMatrix(N);
    printf("矩阵M和N的乘积为:\n");
    MultMatrix(M, N, &Q);
    PrintMatrix(Q);
}

int CreateMatrix(TriSeqMatrix* M)
/*创建稀疏矩阵。要求按照行优先顺序输入非零元素值*/
{
    int i, m, n;
    DataType e;
    int flag;
    printf("请输入稀疏矩阵的行数、列数和非零元素数：");
    scanf("%d,%d,%d", &M->m, &M->n, &M->len);

    if (M->len > MaxSize) {
        return 0;
    }

    for (i = 0; i < M->len; i++) {
        do {
            printf("请以行序顺序输入第%d个非零元素所在的行(0～%d),列(0～%d)元素值:", i,
                   M->m - 1, M->n - 1);
            scanf("%d,%d,%d", &m, &n, &e);
            flag = 0;                       /*初始化标志位*/

            if (m < 0 || m > M->m || n < 0 ||
                n > M->n) { /*如果行号或列号不正确，标志位为1*/
                flag = 1;
            }

            if (i > 0 && m < M->data[i - 1].i || m == M->data[i - 1].i &&
                n <= M->data[i - 1].j) { /*如果输入的顺序不正确，标志位为1*/
                flag = 1;
            }
        } while (flag);

        M->data[i].i = m;
        M->data[i].j = n;
        M->data[i].e = e;
    }

    return 1;
}
void PrintMatrix(TriSeqMatrix M)
/*稀疏矩阵的输出*/
{
    int i;
    printf("稀疏矩阵是%d行×%d列，共%d个非零元素。\n", M.m, M.n, M.len);
    printf("行    列    元素值\n");

    for (i = 0; i < M.len; i++) {
        printf("%2d%6d%8d\n", M.data[i].i, M.data[i].j, M.data[i].e);
    }
}
void MultMatrix(TriSeqMatrix A, TriSeqMatrix B, TriSeqMatrix* C)
/*稀疏矩阵相乘*/
{
    int i, j, k, r, t, p, q, arow, brow, ccol;
    int temp[MaxSize];      /*累加器*/
    int num[MaxSize];

    if (A.n != B.m) {           /*如果矩阵A的列与B的行不相等，则返回*/
        return;
    }

    C->m = A.m;                 /*初始化C的行数、列数和非零元素的个数*/
    C->n = B.n;
    C->len = 0;

    if (A.len * B.len == 0) {   /*只要有一个矩阵的长度为0，则返回*/
        return;
    }

    /*---------------求矩阵B中每一行第一个非零元素的位置------------*/
    for (i = 0; i < B.m; i++) { /*初始化num*/
        num[i] = 0;
    }

    for (k = 0; k < B.len; k++) { /*num存放矩阵B中每一行非零元素的个数*/
        i = B.data[k].i;
        num[i]++;
    }

    B.rpos[0] = 0;

    for (i = 1; i < B.m; i++) { /*rpos存放矩阵B中每一行第一个非零元素的位置*/
        B.rpos[i] = B.rpos[i - 1] + num[i - 1];
    }

    /*---------------求矩阵A中每一行第一个非零元素的位置--------------*/
    for (i = 0; i < A.m; i++) { /*初始化num*/
        num[i] = 0;
    }

    for (k = 0; k < A.len; k++) {
        i = A.data[k].i;
        num[i]++;
    }

    A.rpos[0] = 0;

    for (i = 1; i < A.m; i++) { /*rpos存放矩阵A中每一行第一个非零元素的位置*/
        A.rpos[i] = A.rpos[i - 1] + num[i - 1];
    }

    /*----------------计算两个矩阵的乘积---------------------------------*/
    for (arow = 0; arow < A.m; arow++) { /*依次扫描矩阵A的每一行*/
        for (i = 0; i < B.n; i++) { /*初始化累加器temp*/
            temp[i] = 0;
        }

        C->rpos[arow] = C->len;

        /*对每个非0元处理*/
        if (arow < A.m - 1) {
            t = A.rpos[arow + 1];
        } else {
            t = A.len;
        }

        for (p = A.rpos[arow]; p < t; p++) {
            brow = A.data[p].j;     /*取出A中元素的列号*/

            if (brow < B.m - 1) {
                t = B.rpos[brow + 1];
            } else {
                t = B.len;
            }

            for (q = B.rpos[brow]; q < t; q++) { /*依次取出B中的第brow行，与A中的元素相乘*/
                ccol = B.data[q].j;
                temp[ccol] += A.data[p].e * B.data[q].e; /*把乘积存入temp中*/
            }
        }

        for (ccol = 0; ccol < C->n; ccol++) /*将temp中元素依次赋值给C*/
            if (temp[ccol]) {
                if (++C->len > MaxSize) {
                    return;
                }

                C->data[C->len - 1].i = arow;
                C->data[C->len - 1].j = ccol;
                C->data[C->len - 1].e = temp[ccol];
            }
    }
}
