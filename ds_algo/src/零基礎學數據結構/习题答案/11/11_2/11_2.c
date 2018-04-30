/*包含头文件和顺序表的类型定义*/
#include<stdio.h>
#include<stdlib.h>
#define MaxSize 100
#define IndexSize 20
typedef int KeyType;
typedef struct { /*元素的定义*/
    KeyType key;
} DataType;
typedef struct { /*索引表的类型定义*/
    KeyType maxkey;
    int index;
} IndexTable[IndexSize];
typedef struct { /*顺序表的类型定义*/
    DataType list[MaxSize];
    int length;
} SSTable;
int SeqIndexSearch(SSTable S, IndexTable T, int m, DataType x);
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
void DispList(SSTable S)
/*输出表S中的元素*/
{
    int i;

    for (i = 0; i < S.length; i++) {
        printf("%-4d", S.list[i].key);
    }

    printf("\n");
}
void main()
{
    SSTable S = {{2, 25, 23, 16, 21, 29, 41, 32, 37, 35, 49, 46, 59, 54, 52, 61, 89, 76, 68, 72}, 20};
    IndexTable T = {{25, 0}, {41, 5}, {59, 10}, {89, 15}};
    DataType x = {52};
    int pos;
    printf("表中的元素为:\n");
    DispList(S);

    if ((pos = SeqIndexSearch(S, T, 4, x)) != 0) {
        printf("索引顺序表的查找：关键字52在主表中的位置是：%2d\n", pos);
    } else {
        printf("查找失败！\n");
    }

}
