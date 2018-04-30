
#include<stdio.h>
#define MaxSize 200
typedef int DataType;
typedef struct {        /*三元组类型定义*/
    int i, j;
    DataType e;
} Triple;
typedef struct {        /*矩阵类型定义*/
    Triple data[MaxSize];
    int m, n, len;      /*矩阵的行数，列数和非零元素的个数*/
} TriSeqMatrix;

void PrintMatrix(TriSeqMatrix M)
/*稀疏矩阵的输出*/
{
    int n;
    printf("稀疏矩阵共%d行%d列，有%d个非零元素。\n", M.m, M.n, M.len);
    printf("行  列  元素值\n");

    for (n = 0; n < M.len; n++) {
        printf("%2d%4d%8d\n", M.data[n].i, M.data[n].j, M.data[n].e);
    }
}

void main()
{
    TriSeqMatrix M = {{{1, 1, 4}, {2, 3, 4}, {3, 2, 16}, {4, 3, 26}}, 4, 4, 4}; /*稀疏矩阵的初始化*/
    PrintMatrix(M);                                             /*输出稀疏矩阵*/
}