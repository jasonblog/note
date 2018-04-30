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
void InitSeqList(SqList* L, DataType a[], int start, int n);
void DispList(SqList L);
void Merge(DataType s[], DataType t[], int low, int mid, int high);
void Merge(DataType s[], DataType t[], int low, int mid, int high);

void Merge(DataType s[], DataType t[], int low, int mid, int high)
/*将有序的s[low...mid]和s[mid+1..high]归并为有序的t[low..high]*/
{
    int i, j, k;
    i = low, j = mid + 1, k = low;

    while (i <= mid && j <= high) { /*将s中元素由小到大地合并到t*/
        if (s[i].key <= s[j].key) {
            t[k] = s[i++];
        } else {
            t[k] = s[j++];
        }

        k++;
    }

    while (i <= mid) {      /*将剩余的s[i..mid]复制到t*/
        t[k++] = s[i++];
    }

    while (j <= high) {     /*将剩余的s[j..high]复制到t*/
        t[k++] = s[j++];
    }
}

void MergeSort(DataType s[], DataType t[], int low, int high)
/*2路归并排序，将s[low...high]归并排序并存储到t[low...high]中*/
{
    int mid;
    DataType t2[MaxSize];

    if (low == high) {
        t[low] = s[low];
    } else {
        mid = (low + high) /
              2;     /*将s[low...high]平分为s[low...mid]和s[mid+1...high]*/
        MergeSort(s, t2, low, mid); /*将s[low...mid]归并为有序的t2[low...mid]*/
        MergeSort(s, t2, mid + 1,
                  high); /*将s[mid+1...high]归并为有序的t2[mid+1...high]*/
        Merge(t2, t, low, mid,
              high); /*将t2[low...mid]和t2[mid+1..high]归并到t[low...high]*/
    }
}


void main()
{
    DataType a[] = {37, 22, 43, 32, 19, 12, 89, 26, 48, 92};
    DataType b[MaxSize];
    int i, n = 10;
    SqList L;
    /*归并排序*/
    InitSeqList(&L, a, 0, n); /*将数组a[0...n-1]初始化为顺序表L*/
    printf("归并排序前：");
    DispList(L);
    MergeSort(L.data, b, 1, n);
    InitSeqList(&L, b, 1, n); /*将数组b[1...n]初始化为顺序表L*/
    printf("归并排序结果：");
    DispList(L);

}
void InitSeqList(SqList* L, DataType a[], int start, int n)
/*顺序表的初始化*/
{
    int i, k;

    for (k = 1, i = start; i < start + n; i++, k++) {
        L->data[k] = a[i];
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
