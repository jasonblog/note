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
void InsertSort(SqList* L);
void ShellInsert(SqList* L, int c);
void ShellInsertSort(SqList* L, int delta[], int m);
void BinInsertSort(SqList* L);
void DispList(SqList L, int n);
void InsertSort(SqList* L)
/*直接插入排序*/
{
    int i, j;
    DataType t;

    for (i = 1; i < L->length;
         i++) { /*前i个元素已经有序，从第i+1个元素开始与前i个的有序的关键字比较*/
        t = L->data[i + 1];         /*取出当前待排序的元素*/
        j = i;

        while (j > -1 && t.key < L->data[j].key) { /*寻找当前元素的合适位置*/
            L->data[j + 1] = L->data[j];
            j--;
        }

        L->data[j + 1] = t;         /*将当前元素插入合适的位置*/
    }
}
void BinInsertSort(SqList* L)
/*折半插入排序*/
{
    int i, j, mid, low, high;
    DataType t;

    for (i = 1; i < L->length;
         i++) { /*前i个元素已经有序，从第i+1个元素开始与前i个的有序的关键字比较*/
        t = L->data[i + 1];         /*取出第i+1个元素，即待排序的元素*/
        low = 1, high = i;

        while (low <= high) {       /*利用折半查找思想寻找当前元素的合适位置*/
            mid = (low + high) / 2;

            if (L->data[mid].key > t.key) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        for (j = i; j >= low; j--) { /*移动元素，空出要插入的位置*/
            L->data[j + 1] = L->data[j];
        }

        L->data[low] = t;       /*将当前元素插入合适的位置*/
    }
}
void ShellInsert(SqList* L, int c)
/*对顺序表L进行一次希尔排序，delta是增量*/
{
    int i, j;
    DataType t;

    for (i = c + 1; i <= L->length;
         i++) { /*将距离为delta的元素作为一个子序列进行排序*/
        if (L->data[i].key < L->data[i - c].key) { /*如果后者小于前者，则需要移动元素*/
            t = L->data[i];

            for (j = i - c; j > 0 && t.key < L->data[j].key; j = j - c) {
                L->data[j + c] = L->data[j];
            }

            L->data[j + c] = t;             /*依次将元素插入到正确的位置*/
        }
    }
}
void ShellInsertSort(SqList* L, int delta[], int m)
/*希尔排序，每次调用算法ShellInsert,delta是存放增量的数组*/
{
    int i;

    for (i = 0; i < m; i++) { /*进行m次希尔插入排序*/
        ShellInsert(L, delta[i]);
    }
}
void main()
{
    DataType a[] = {56, 22, 67, 32, 59, 12, 89, 26, 48, 37};
    int delta[] = {5, 3, 1};
    int i, n = 10, m = 3;
    SqList L;
    /*直接插入排序*/
    InitSeqList(&L, a, n);
    printf("排序前：");
    DispList(L, n);
    InsertSort(&L);
    printf("直接插入排序结果：");
    DispList(L, n);
    /*折半插入排序*/
    InitSeqList(&L, a, n);
    printf("排序前：");
    DispList(L, n);
    BinInsertSort(&L);
    printf("折半插入排序结果：");
    DispList(L, n);
    /*希尔排序*/
    InitSeqList(&L, a, n);
    printf("排序前：");
    DispList(L, n);
    ShellInsertSort(&L, delta, m);
    printf("希尔排序结果：");
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