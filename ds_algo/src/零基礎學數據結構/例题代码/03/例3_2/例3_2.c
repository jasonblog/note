#include<stdio.h>                   /*包含输入输出头文件*/
#define ListSize 100
typedef int DataType;
typedef struct {
    DataType list[ListSize];
    int length;
} SeqList;
#include"SeqList.h"                 /*包含顺序表实现文件*/
void MergeList(SeqList A, SeqList B,
               SeqList* C); /*合并顺序表A和B中元素的函数声明*/
void main()
{
    int i, flag;
    DataType a[] = {6, 11, 11, 23};
    DataType b[] = {2, 10, 12, 12, 21};
    DataType e;
    SeqList A, B, C;                /*声明顺序表A，B和C*/
    InitList(&A);                   /*初始化顺序表A*/
    InitList(&B);                   /*初始化顺序表B*/
    InitList(&C);                   /*初始化顺序表C*/

    for (i = 1; i <= sizeof(a) / sizeof(a[0]);
         i++) {   /*将数组a中的元素插入到顺序表A中*/
        if (InsertList(&A, i, a[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    for (i = 1; i <= sizeof(b) / sizeof(b[0]);
         i++) {   /*将数组b中元素插入到顺序表B中*/
        if (InsertList(&B, i, b[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    printf("顺序表A中的元素：\n");

    for (i = 1; i <= A.length; i++) { /*输出顺序表A中的每个元素*/
        flag = GetElem(A, i, &e);   /*返回顺序表A中的每个元素到e中*/

        if (flag == 1) {
            printf("%4d", e);
        }
    }

    printf("\n");
    printf("顺序表B中的元素：\n");

    for (i = 1; i <= B.length; i++) { /*输出顺序表B中的每个元素*/
        flag = GetElem(B, i, &e);   /*返回顺序表B中的每个元素到e中*/

        if (flag == 1) {
            printf("%4d", e);
        }
    }

    printf("\n");
    printf("将在A中出现B的元素合并后C中的元素：\n");
    MergeList(A, B, &C);                /*将在顺序表A和B中的元素合并*/

    for (i = 1; i <= C.length; i++) {   /*显示输出合并后C中所有元素*/
        flag = GetElem(C, i, &e);

        if (flag == 1) {
            printf("%4d", e);
        }
    }

    printf("\n");

}
void MergeList(SeqList A, SeqList B, SeqList* C)
/*合并顺序表A和B的元素到C中，并保持元素非递减排序*/
{
    int i, j, k;
    DataType e1, e2;
    i = 1;
    j = 1;
    k = 1;

    while (i <= A.length && j <= B.length) {
        GetElem(A, i, &e1);             /*取出顺序表A中的元素*/
        GetElem(B, j, &e2);             /*取出顺序表B中的元素*/

        if (e1 <= e2) {                 /*比较顺序表A和顺序表B中的元素*/
            InsertList(C, k, e1);       /*将较小的一个插入到C中*/
            i++;                        /*往后移动一个位置，准备比较下一个元素*/
            k++;
        } else {
            InsertList(C, k, e2);       /*将较小的一个插入到C中*/
            j++;                        /*往后移动一个位置，准备比较下一个元素*/
            k++;
        }

    }

    while (i <= A.length) {             /*如果A中元素还有剩余，这时B中已经没有元素*/
        GetElem(A, i, &e1);
        InsertList(C, k, e1);           /*将A中剩余元素插入到C中*/
        i++;
        k++;
    }

    while (j <= B.length) {             /*如果B中元素还有剩余，这时A中已经没有元素*/
        GetElem(B, j, &e2);
        InsertList(C, k, e2);           /*将B中剩余元素插入到C中*/
        j++;
        k++;
    }

    C->length = A.length + B.length;    /*C的表长等于A和B的表长的和*/
}
