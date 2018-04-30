

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
void DispList2(SqList L, int pivot, int count);
void DispList3(SqList L, int count);
void HeapSort(SqList* H);
void BubbleSort(SqList* L, int n);
void BubbleSort(SqList* L, int n)
/*冒泡排序*/
{
    int i, j, flag;
    DataType t;
    static int count = 1;

    for (i = 1; i <= n - 1 && flag; i++) { /*需要进行n-1趟排序*/
        flag = 0;

        for (j = 1; j <= n - i; j++) /*每一趟排序需要比较n-i次*/
            if (L->data[j].key > L->data[j + 1].key) {
                t = L->data[j];
                L->data[j] = L->data[j + 1];
                L->data[j + 1] = t;
                flag = 1;
            }

        DispList3(*L, count);
        count++;
    }
}
void QSort(SqList* L, int low, int high)
/*对顺序表L进行快速排序*/
{
    int pivot;
    static count = 1;

    if (low < high) {                       /*如果元素序列的长度大于1*/
        pivot = Partition(L, low, high);    /*将待排序序列L.r[low..high]划分为两部分*/
        DispList2(*L, pivot, count);        /*输出每次划分的结果*/
        count++;
        QSort(L, low, pivot -
              1);           /*对左边的子表进行递归排序，pivot是枢轴位置*/
        QSort(L, pivot + 1, high);          /*对右边的子表进行递归排序 */
    }
}

void QuickSort(SqList* L)
/* 对顺序表L作快速排序*/
{
    QSort(L, 1, (*L).length);
}

int Partition(SqList* L, int low, int high)
/*对顺序表L.r[low..high]的元素进行一趟排序，使枢轴前面的元素关键字小于
枢轴元素的关键字，枢轴后面的元素关键字大于等于枢轴元素的关键字，并返回枢轴位置*/
{
    DataType t;
    KeyType pivotkey;
    pivotkey = (*L).data[low].key;  /*将表的第一个元素作为枢轴元素*/
    t = (*L).data[low];

    while (low < high) {            /*从表的两端交替地向中间扫描*/
        while (low < high && (*L).data[high].key >= pivotkey) { /*从表的末端向前扫描*/
            high--;
        }

        if (low < high) {           /*将当前high指向的元素保存在low位置*/
            (*L).data[low] = (*L).data[high];
            low++;
        }

        while (low < high && (*L).data[low].key <= pivotkey) { /*从表的始端向后扫描*/
            low++;
        }

        if (low < high) {           /*将当前low指向的元素保存在high位置*/
            (*L).data[high] = (*L).data[low];
            high--;
        }

        (*L).data[low] = t;         /*将枢轴元素保存在low=high的位置*/

    }

    return low;                     /*返回枢轴所在位置*/
}

void DispList2(SqList L, int pivot, int count)
{
    int i;
    printf("第%d趟排序结果:[", count);

    for (i = 1; i < pivot; i++) {
        printf("%-4d", L.data[i].key);
    }

    printf("]");
    printf("%3d ", L.data[pivot].key);
    printf("[");

    for (i = pivot + 1; i <= L.length; i++) {
        printf("%-4d", L.data[i].key);
    }

    printf("]");
    printf("\n");

}
void DispList3(SqList L, int count)
/*输出表中的元素*/
{
    int i;
    printf("第%d趟排序结果:", count);

    for (i = 1; i <= L.length; i++) {
        printf("%4d", L.data[i].key);
    }

    printf("\n");
}
void main()
{
    DataType a[] = {37, 22, 43, 32, 19, 12, 89, 26, 48, 92};
    int i, n = 10;
    SqList L;
    /*冒泡排序*/
    InitSeqList(&L, a, n);
    printf("冒泡排序前：");
    DispList(L);
    BubbleSort(&L, n);
    printf("冒泡排序结果：");
    DispList(L, n);
    /*快速排序*/
    InitSeqList(&L, a, n);
    printf("快速排序前：");
    DispList(L, n);
    QuickSort(&L, n);
    printf("快速排序结果：");
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