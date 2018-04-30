#include<stdio.h>
#include"SeqList.h"

void CombAB(SeqList* A, SeqList B)
/*合并顺序表A和B，将B中的元素插入到A中，使A仍然是递增有序的顺序表*/
{
    int la = A->length, lb = B.length, i = 0;

    while (i < lb) { /*从B的最后一个元素往前与A中的元素第一个元素向后进行比较*/
        if (la == 0 ||
            A->list[la - 1] <
            B.list[i]) { /*如果A中的元素小于B中的元素，则将B插入到A中正确的位置*/
            A->list[la + lb - i - 1] = B.list[i];
            i++;
        } else {                /*如果A中的元素大于B中的元素，则将A中元素后移给，然后继续比较*/
            A->list[la + lb - i - 1] = A->list[la - 1];
            la--;
        }
    }

    A->length = A->length + B.length; /*修改A的长度*/
}
void main()
{
    SeqList A, B;
    int i, m, n;
    DataType a[] = {3, 7, 8, 12, 15};
    DataType b[] = {9, 5, 4, 1}, e;
    n = 5, m = 4;
    InitList(&A);
    InitList(&B);

    for (i = 0; i < n; i++) {
        InsertList(&A, i + 1, a[i]);
    }

    for (i = 0; i < m; i++) {
        InsertList(&B, i + 1, b[i]);
    }

    printf("顺序表A的元素依次为：");

    for (i = 0; i < A.length; i++) {
        GetElem(A, i + 1, &e);
        printf("%4d", e);
    }

    printf("\n");
    printf("顺序表B的元素依次为：");

    for (i = 0; i < B.length; i++) {
        GetElem(B, i + 1, &e);
        printf("%4d", e);
    }

    printf("\n");
    CombAB(&A, B);
    printf("顺序表A和顺序表B合并后A中的元素依次为：");

    for (i = 0; i < A.length; i++) {
        GetElem(A, i + 1, &e);
        printf("%4d", e);
    }

    printf("\n");
}
