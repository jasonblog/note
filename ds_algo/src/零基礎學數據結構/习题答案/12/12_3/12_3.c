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
void Split(SqList* L, int low, int high, int* i);
void QuickSort(SqList* L)
/*利用栈实现快速排序(非递归)*/
{
    int i = 1, j = 1, k = 1;
    int stack[MaxSize][2], top = -1;
    int low = 1, high = L->length;
    top++;
    stack[top][0] = low;
    stack[top][1] = high;

    while (top >= 0) {
        low = stack[top][0];    /*将表的起始位置出栈，然后将当前表进行分割为两个更小的子表*/
        high = stack[top][1];
        top--;
        Split(L, low, high, &i); /*将表分割为两个部分(排序)，并将分割点返回*/

        if (low < high) {       /*将表的起始位置保存到栈中*/
            top++;
            stack[top][0] = low;
            stack[top][1] = i - 1;
            top++;
            stack[top][0] = i + 1;
            stack[top][1] = high;
        }
    }
}
void Split(SqList* L, int low, int high, int* i)
/*对顺序表L.r[low..high]的元素进行一趟排序,以L->data[i].key将表分割为两个部分*/
{
    DataType t;
    int j;
    *i = low;
    j = high;
    t = (*L).data[*i];

    while (*i < j) {            /*从表的两端交替地向中间扫描*/
        while (*i < j && (*L).data[j].key >= t.key) { /*从表的末端向前扫描*/
            j--;
        }

        if (*i < j) {           /*将当前j指向的元素保存在i位置*/
            (*L).data[*i] = (*L).data[j];
            (*i)++;
        }

        while (*i < j && (*L).data[*i].key <= t.key) { /*从表的始端向后扫描*/
            (*i)++;
        }

        if (*i < j) {           /*将当前i指向的元素保存在j位置*/
            (*L).data[j] = (*L).data[*i];
            j--;
        }

    }

    (*L).data[*i] = t;

}
void main()
{
    DataType a[] = {41, 32, 34, 5, 12, 43, 67, 98, 21, 73};
    int n = 10;
    SqList L;
    InitSeqList(&L, a, n);
    printf("排序前L的元素：");
    DispList(L);
    QuickSort(&L);
    printf("非递归快速排序后L的元素：");
    DispList(L);
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
