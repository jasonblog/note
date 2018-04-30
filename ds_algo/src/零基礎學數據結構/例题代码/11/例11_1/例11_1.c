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
typedef struct { /*索引表的类型定义*/
    KeyType maxkey;
    int index;
} IndexTable[IndexSize];
int SeqSearch(SSTable S, DataType x);
int BinarySearch(SSTable S, DataType x);
int SeqIndexSearch(SSTable S, IndexTable T, int m, DataType x);
int SeqSearch(SSTable S, DataType x)
/*在顺序表中查找关键字为x的元素，如果找到返回该元素在表中的位置，否则返回0*/
{
    int i = 0;

    while (i < S.length &&
           S.list[i].key != x.key) { /*从顺序表的第一个元素开始比较*/
        i++;
    }

    if (S.list[i].key == x.key) {
        return i + 1;
    } else {
        return 0;
    }
}
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

int SeqIndexSearch(SSTable S, IndexTable T, int m, DataType x)
/*在主表S中查找关键字为x的元素，T为索引表。如果找到返回该元素在表中的位置，否则返回0*/
{
    int i, j, bl;

    for (i = 0; i < m; i++) /*通过索引表确定要查找元素所在的单元*/
        if (T[i].maxkey >= x.key) {
            break;
        }

    if (i >= m) {       /*如果要查找的元素不在索引顺序表中，则返回0*/
        return 0;
    }

    j = T[i].index;     /*要查找的元素在的主表的第j单元*/

    if (i < m - 1) {    /*bl为第j单元的长度*/
        bl = T[i + 1].index - T[i].index;
    } else {
        bl = S.length - T[i].index;
    }

    while (j < T[i].index + bl)
        if (S.list[j].key ==
            x.key) { /*如果找到关键字，则返回该关键字在主表中所在的位置*/
            return j + 1;
        } else {
            j++;
        }

    return 0;


}
void main()
{
    SSTable S1 = {{123, 23, 34, 6, 8, 355, 32, 67}, 8};
    SSTable S2 = {{11, 23, 32, 35, 39, 41, 45, 67}, 8};
    SSTable S3 = {{6, 12, 23, 16, 21, 26, 41, 32, 37, 35, 48, 46, 49, 47, 52, 61, 59, 76, 68, 72}, 20};
    IndexTable T = {{23, 0}, {41, 5}, {52, 10}, {76, 15}};
    DataType x = {32};
    int pos;

    if ((pos = SeqSearch(S1, x)) != 0) {
        printf("顺序表的查找：关键字32在主表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

    if ((pos = BinarySearch(S2, x)) != 0) {
        printf("折半查找：关键字32在主表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

    if ((pos = SeqIndexSearch(S3, T, 4, x)) != 0) {
        printf("索引顺序表的查找：关键字32在主表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

}
