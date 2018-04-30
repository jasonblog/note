/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "string.h"
/*宏定义和链栈类型定义*/
typedef char DataType;
#include"LinkStack.h"                   /*包含链栈实现文件*/
int Match(DataType e, DataType ch);
void main()
{
    LinkStack S;
    char* p;
    DataType e;
    DataType ch[60];
    InitStack(&S);                  /*初始化链栈*/
    printf("请输入带括号的表达式('{}','[]','()').\n");
    gets(ch);
    p = ch;

    while (*p) {
        switch (*p) {
        case '(':
        case '[':
        case '{':
            PushStack(S, *p++);
            break;

        case ')':
        case ']':
        case '}':
            if (StackEmpty(S)) {
                printf("缺少左括号.\n");
                return;
            } else {
                GetTop(S, &e);

                if (Match(e, *p)) {
                    PopStack(S, &e);
                } else {
                    printf("左右括号不配对.\n");
                    return;
                }
            }

        default:
            p++;
        }
    }

    if (StackEmpty(S)) {
        printf("括号匹配.\n");
    } else {
        printf("缺少右括号.\n");
    }
}


int Match(DataType e, DataType ch)
{
    if (e == '(' && ch == ')') {
        return 1;
    } else if (e == '[' && ch == ']') {
        return 1;
    } else if (e == '{' && ch == '}') {
        return 1;
    } else {
        return 0;
    }

}