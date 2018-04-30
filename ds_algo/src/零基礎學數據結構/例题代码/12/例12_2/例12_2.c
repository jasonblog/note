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
void DispList(SqList L, int n);
void AdjustHeap(SqList* H, int s, int m);
void CreateHeap(SqList* H, int n);
void HeapSort(SqList* H);
void SelectSort(SqList* L, int n);

void SelectSort(SqList* L, int n)
/*简单选择排序*/
{
    int i, j, k;
    DataType t;

    /*将第i个元素的关键字与后面[i+1...n]个元素的关键字比较，将关键字最小的的元素放在第i个位置*/
    for (i = 1; i <= n - 1; i++) {
        j = i;

        for (k = i + 1; k <= n; k++) /*关键字最小的元素的序号为j*/
            if (L->data[k].key < L->data[j].key) {
                j = k;
            }

        if (j != i) {       /*如果序号i不等于j，则需要将序号i和序号j的元素交换*/
            t = L->data[i];
            L->data[i] = L->data[j];
            L->data[j] = t;
        }
    }
}
void CreateHeap(SqList* H, int n)
/*建立大顶堆*/
{
    int i;

    for (i = n / 2; i >= 1; i--) { /*从序号n/2开始建立大顶堆*/
        AdjustHeap(H, i, n);
    }
}
void AdjustHeap(SqList* H, int s, int m)
/*调整H.data[s...m]的关键字，使其成为一个大顶堆*/
{
    DataType t;
    int j;
    t = (*H).data[s];                       /*将根结点暂时保存在t中*/

    for (j = 2 * s; j <= m; j *= 2) {
        if (j < m &&
            (*H).data[j].key < (*H).data[j + 1].key) { /*沿关键字较大的孩子结点向下筛选*/
            j++;    /*j为关键字较大的结点的下标*/
        }

        if (t.key >
            (*H).data[j].key) { /*如果孩子结点的值小于根结点的值，则不进行交换*/
            break;
        }

        (*H).data[s] = (*H).data[j];
        s = j;
    }

    (*H).data[s] = t;                               /*将根结点插入到正确位置*/
}

void HeapSort(SqList* H)
/*对顺序表H进行堆排序*/
{
    DataType t;
    int i;
    CreateHeap(H, H->length);   /*创建堆*/

    for (i = (*H).length; i > 1; i--) { /*将堆顶元素与最后一个元素交换，重新调整堆*/
        t = (*H).data[1];
        (*H).data[1] = (*H).data[i];
        (*H).data[i] = t;
        AdjustHeap(H, 1, i - 1); /*将(*H).data[1..i-1]调整为大顶堆*/
    }
}

void main()
{
    DataType a[] = {56, 22, 67, 32, 59, 12, 89, 26, 48, 37};
    int delta[] = {5, 3, 1};
    int i, n = 10, m = 3;
    SqList L;
    /*简单选择排序*/
    InitSeqList(&L, a, n);
    printf("排序前：");
    DispList(L, n);
    SelectSort(&L, n);
    printf("简单选择排序结果：");
    DispList(L, n);
    /*堆排序*/
    InitSeqList(&L, a, n);
    printf("排序前：");
    DispList(L, n);
    HeapSort(&L, n);
    printf("堆排序结果：");
    DispList(L, n);
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
void DispList(SqList L, int n)
/*输出表中的元素*/
{
    int i;

    for (i = 1; i <= n; i++) {
        printf("%4d", L.data[i].key);
    }

    printf("\n");
}