/*包含头文件*/
#include<stdio.h>
#include<stdlib.h>
/*类型定义*/
#define StackSize 100
typedef char DataType;
typedef struct {
    DataType stack[StackSize];
    int top;
} SeqStack;
void InitStack(SeqStack* S)
/*栈的初始化操作*/
{
    S->top = 0;
}
int PushStack(SeqStack* S, DataType e)
/*入栈操作。进栈成功返回1，否则返回0*/
{
    if (S->top >= StackSize - 1) { /*在进栈操作之前，判断共享栈是否为空*/
        return 0;
    } else {
        S->stack[S->top] = e;   /*元素进栈*/
        S->top++;               /*修改栈顶指针*/
        return 1;
    }
}

int PopStack(SeqStack* S, DataType* e)
/*将栈顶元素出栈。如果栈为空，则返回0，否则将栈顶元素出栈并赋值给e并返回1*/
{

    if (S->top == 0) {          /*如果栈为空，则返回0，表示出栈操作失败*/
        return 0;
    } else {
        S->top--;               /*修改栈顶指针*/
        *e = S->stack[S->top];  /*将出栈的元素赋值给e*/
        return 1;
    }
}
int StackEmpty(SeqStack S)
/*判断栈是否为空。如果栈为空，则返回1，否则返回0*/
{
    if (S.top == 0) {
        return 1;
    } else {
        return 0;
    }
}
int GetTop(SeqStack S, DataType* e)
/*返回栈顶元素。*/
{
    if (S.top == 0) { /*如果栈为空，则返回0，表示出栈操作失败*/
        return 0;
    } else {
        *e = S.stack[S.top - 1];    /*将出栈的元素赋值给e*/
    }

}
void main()
{
    SeqStack S;                     /*定义一个栈*/
    int i;
    DataType str[StackSize];
    DataType e;
    InitStack(&S);                  /*初始化栈*/
    printf("请输入字符串");
    gets(str);

    for (i = 0; i < strlen(str); i++) { /*将字符串str中的字符依次进栈*/
        if (PushStack(&S, str[i], 0) == 0) {
            printf("栈已满，不能进栈！");
            return;
        }
    }

    if (GetTop(S, &e) == 0) {   /*取栈顶元素*/
        printf("栈已空");
        return;
    }

    printf("栈S的栈顶元素是：%c\n", e);
    printf("将栈S出栈的元素次序是：");

    while (!StackEmpty(S)) {    /*依次将栈中元素出栈并输出*/
        PopStack(&S, &e);
        printf("%4c", e);
    }

    printf("\n");
}

