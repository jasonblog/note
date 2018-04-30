/*包含头文件和顺序表的类型定义*/
#include<stdio.h>
#include<stdlib.h>
#define MaxSize 100
#define IndexSize 20
typedef int KeyType;
typedef struct { /*元素的定义*/
    KeyType key;
} DataType;
typedef struct { /*顺序表的类型定义*/
    DataType list[MaxSize];
    int length;
} SSTable;
int BinarySearch(SSTable S, DataType x);
int BinarySearch(SSTable S, DataType x)
/*在有序顺序表中折半查找关键字为x的元素，如果找到返回该元素在表中的位置，否则返回0*/
{
    int low, high, mid;
    low = 0, high = S.length - 1;   /*设置待查找元素范围的下界和上界*/

    while (low <= high) {
        mid = (low + high) / 2;

        if (S.list[mid].key == x.key) { /*如果找到元素，则返回该元素所在的位置*/
            return mid + 1;
        } else if (S.list[mid].key <
                   x.key) { /*如果mid所指示的元素小于关键字，则修改low指针*/
            low = mid + 1;
        } else if (S.list[mid].key >
                   x.key) { /*如果mid所指示的元素大于关键字，则修改high指针*/
            high = mid - 1;
        }
    }

    return 0;
}
int BinarySearch2(SSTable S, int low, int high, DataType x)
/*递归实现折半查找*/
{
    int mid;

    if (low < high) {
        mid = (low + high) / 2;

        if (S.list[mid].key == x.key) { /*如果找到元素，则返回该元素所在的位置*/
            return mid + 1;
        } else if (S.list[mid].key >
                   x.key) { /*如果mid所指示的元素小于关键字，则在表的上半部分查找*/
            return BinarySearch2(S, low, mid - 1, x);
        } else {                        /*如果mid所指示的元素大于关键字，则在表的下半部分查找*/
            return BinarySearch2(S, mid + 1, high, x);
        }
    }

    return 0;

}
void DispList(SSTable S)
{
    int i;

    for (i = 0; i < S.length; i++) {
        printf("%4d", S.list[i].key);
    }

    printf("\n");
}
void main()
{
    SSTable S = {{12, 23, 34, 43, 49, 67, 78, 98}, 8};
    DataType x = {67};
    int pos;
    printf("表S中的元素为：\n");
    DispList(S);

    if ((pos = BinarySearch(S, x)) != 0) {
        printf("非递归实现折半查找：关键字67在表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

    if ((pos = BinarySearch2(S, 0, 7, x)) != 0) {
        printf("递归实现折半查找：关键字67在表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

}