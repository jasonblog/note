/*包含头文件*/
#include<stdio.h>
#include<stdlib.h>
/*类型定义*/
#define StackSize 100
typedef char DataType;
#include "SSeqStack.h"      /*包含栈的基本类型定义和基本操作实现*/
int GetTop(SSeqStack S, DataType* e, int flag);
int StackEmpty(SSeqStack S, int flag);
void main()
{
    SSeqStack S1, S2;           /*定义一个栈*/
    int i;
    DataType a[] = {'a', 'b', 'c', 'd', 'e'};
    DataType b[] = {'x', 'y', 'z', 'r'};
    DataType e1, e2;
    InitStack(&S1);                 /*初始化栈*/
    InitStack(&S2);

    for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) { /*将数组a中元素依次进栈*/
        if (PushStack(&S1, a[i], 0) == 0) {
            printf("栈已满，不能进栈！");
            return;
        }
    }

    for (i = 0; i < sizeof(b) / sizeof(b[0]); i++) { /*将数组a中元素依次进栈*/
        if (PushStack(&S2, b[i], 1) == 0) {
            printf("栈已满，不能进栈！");
            return;
        }
    }

    if (GetTop(S1, &e1, 0) == 0) {
        printf("栈已空");
        return;
    }

    if (GetTop(S2, &e2, 1) == 0) {
        printf("栈已空");
        return;
    }

    printf("栈S1的栈顶元素是：%c，栈S2的栈顶元素是:%c\n", e1, e2);
    printf("S1出栈的元素次序是：");
    i = 0;

    while (!StackEmpty(S1, 0)) { /*将数组a中元素依次进栈*/
        PopStack(&S1, &e1, 0);
        printf("%4c", e1);
    }

    printf("\n");
    printf("S2出栈的元素次序是：");

    while (!StackEmpty(S2, 1)) { /*将数组a中元素依次进栈*/
        PopStack(&S2, &e2, 1);
        printf("%4c", e2);
    }

    printf("\n");
}
int GetTop(SSeqStack S, DataType* e, int flag)
/*取栈顶元素。将栈顶元素值返回给e，并返回1表示成功；否则返回0表示失败。*/
{
    switch (flag) {
    case 0:
        if (S.top[0] == 0) {
            return 0;
        }

        *e = S.stack[S.top[0] - 1];
        break;

    case 1:
        if (S.top[1] == StackSize - 1) {
            return 0;
        }

        *e = S.stack[S.top[1] + 1];
        break;

    default:
        return 0;
    }

    return 1;
}
int StackEmpty(SSeqStack S, int flag)
{
    switch (flag) {
    case 0:
        if (S.top[0] == 0) {
            return 1;
        }

        break;

    case 1:
        if (S.top[1] == StackSize - 1) {
            return 1;
        }

        break;

    default:
        return 0;
    }

    return 0;
}
