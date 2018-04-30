/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "string.h"
/*宏定义和链栈类型定义*/
typedef char DataType;
#include"SeqStack.h"                    /*包含链栈实现文件*/
void LineEdit();
void LineEdit()
{
    SeqStack S;
    char ch;
    DataType e;
    DataType a[50];
    int i, j = 0;
    InitStack(&S);
    printf("输入字符序列(#表示前一个字符无效，@表示当前行字符无效).\n");
    ch = getchar();

    while (ch != '\n') {
        switch (ch) {
        case '#':
            if (!StackEmpty(S)) {
                PopStack(&S, &ch);
            }

            break;

        case '@':
            ClearStack(&S);
            break;

        default:
            PushStack(&S, ch);
        }

        ch = getchar();
    }

    while (!StackEmpty(S)) {
        PopStack(&S, &e);
        a[j++] = e;
    }

    for (i = j - 1; i >= 0; i--) {
        printf("%c", a[i]);
    }

    printf("\n");
    ClearStack(&S);
}

void main()
{
    LineEdit();

}