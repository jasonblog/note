#include<stdio.h>                   /*包含输入输出头文件*/
#include"SeqList.h"                 /*包含顺序表实现文件*/
void SplitSeqList(SeqList* L);      /*调整顺序表L*/
void main()
{
    int i, flag, n;
    DataType e;
    SeqList L;
    int a[] = {-7, 0, 5, -8, 9, -4, 3, -2};
    InitList(&L);                   /*初始化顺序表L*/
    n = sizeof(a) / sizeof(a[0]);

    for (i = 1; i <= n; i++) {      /*将数组a的元素插入到顺序表L中*/
        if (InsertList(&L, i, a[i - 1]) == 0) {
            printf("位置不合法");
            return;
        }
    }

    printf("顺序表L中的元素：\n");

    for (i = 1; i <= L.length; i++) { /*输出顺序表L中的每个元素*/
        flag = GetElem(L, i, &e);   /*返回顺序表L中的每个元素到e中*/

        if (flag == 1) {
            printf("%4d", e);
        }
    }

    printf("\n");
    printf("将顺序表L调整后（左边元素>=0,右边元素<0）：\n");
    SplitSeqList(&L);                   /*调整顺序表*/

    for (i = 1; i <= L.length; i++) {   /*输出调整后顺序表L中所有元素*/
        flag = GetElem(L, i, &e);

        if (flag == 1) {
            printf("%4d", e);
        }
    }

    printf("\n");
}
void SplitSeqList(SeqList* L)
/*调整顺序表L，使元素的左边是大于等于0的元素，右边是小于0的元素*/
{
    int i, j;                   /*定义两个指示器i和j*/
    DataType e;
    i = 0, j = (*L).length - 1; /*指示器i和j分别指示顺序表的左端和右端元素*/

    while (i < j) {
        while (L->list[i] >= 0) { /*i遇到大于等于0的元素略过*/
            i++;
        }

        while (L->list[j] < 0) { /*j遇到小于0的元素略过*/
            j--;
        }

        if (i < j) {            /*调整元素的i和j指示的元素位置*/
            e = L->list[i];
            L->list[i] = L->list[j];
            L->list[j] = e;
        }
    }
}