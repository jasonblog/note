/*包含头文件*/
#include<stdio.h>
#include<stdlib.h>
#define MaxSize 50
typedef int KeyType;
typedef struct { /*数据元素类型定义*/
    KeyType key;/*关键字*/
} DataType;
typedef struct { /*顺序表类型定义*/
    DataType data[MaxSize];
    int length;
} SqList;
void InitSeqList(SqList* L, DataType a[], int n);
void DispList(SqList L);
void Sort(SqList A, SqList B, SqList* C)
/*对有序的顺序表A和B合并，合并后保存在C中，并使其继续有序*/
{
    int i = 1, j = 1, k = 1;
    C->length = A.length + B.length;

    while (i <= A.length && j <= B.length) {
        if (A.data[i].key <=
            B.data[j].key) { /*如果A中元素的关键字小于等于B中的关键字，则将A中的元素存入到C中*/
            C->data[k] = A.data[i];
            i++;
        } else {                        /*如果A中元素的关键字大于等于B中的关键字，则将B中的元素存入到C中*/
            C->data[k] = B.data[j];
            j++;
        }

        k++;
    }

    while (i <= A.length) {     /*将A中剩余元素存入到C中*/
        C->data[k] = A.data[i];
        k++;
        i++;
    }

    while (j <= B.length) {     /*将B中剩余元素存入到C中*/
        C->data[k] = B.data[j];
        k++;
        j++;
    }
}

void main()
{
    DataType a[] = {4, 8, 34, 56, 89, 103};
    DataType b[] = {23, 45, 78, 90};
    int n = 6, m = 4;
    SqList A, B, C;
    InitSeqList(&A, a, n);
    InitSeqList(&B, b, m);
    printf("排序前A的元素：");
    DispList(A);
    printf("排序前B的元素：");
    DispList(B);
    Sort(A, B, &C);
    printf("将A和B合并后C中的元素：");
    DispList(C);
}
void InitSeqList(SqList* L, DataType a[], int n)
/*顺序表的初始化*/
{
    int i;

    for (i = 1; i <= n; i++) {
        L->data[i] = a[i - 1];
    }

    L->length = n;

}
void DispList(SqList L)
/*输出表中的元素*/
{
    int i;

    for (i = 1; i <= L.length; i++) {
        printf("%4d", L.data[i].key);
    }

    printf("\n");
}