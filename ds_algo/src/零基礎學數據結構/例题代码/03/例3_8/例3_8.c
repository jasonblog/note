/*包含头文件*/
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
/*类型定义*/
typedef char DataType;
#define ListSize 10
typedef struct {    /*静态链表结点类型定义*/
    DataType data;
    int cur;
} SListNode;
typedef struct {    /*静态链表类型定义*/
    SListNode list[ListSize];
    int av;
} SLinkList;
#include "SLinkList.h"
/*函数声明*/
void PrintDList(SLinkList L, int n);
void main()
{
    SLinkList L;
    int i, len;
    int pos;
    char e;
    DataType a[] = {'A', 'B', 'C', 'D', 'E', 'F', 'H'};
    DataType a1[] = {1, 2, 3, 4, 5, 6, 7, 8};
    len = sizeof(a) / sizeof(a[0]);
    InitSList(&L);

    for (i = 1; i <= len; i++) {
        InsertSList(&L, i, a[i - 1]);
    }

    printf("静态链表中的元素:");
    PrintDList(L, len);
    printf("输入要插入的元素及位置:");
    scanf("%c", &e);
    getchar();
    scanf("%d", &pos);
    getchar();
    InsertSList(&L, pos, e);
    printf("插入元素后静态链表中的元素:");
    PrintDList(L, len + 1);

    printf("输入要删除元素的位置:");
    scanf("%d", &pos);
    getchar();
    DeleteSList(&L, pos, &e);
    printf("删除的元素是:");
    printf("%c\n", e);
    printf("删除元素后静态链表中的元素:");
    PrintDList(L, len);
}
void PrintDList(SLinkList L, int n)
/*输出双向循环链表中的每一个元素*/
{
    int j, k;
    k = L.list[0].cur;

    for (j = 1; j <= n; j++) {
        printf("%4c", L.list[k].data);
        k = L.list[k].cur;
    }

    printf("\n");

}
