#define StackSize 100
typedef struct {
    DataType stack[StackSize];
    int top;
} SeqStack;

void InitStack(SeqStack* S)
/*将栈初始化为空栈只需要把栈顶指针top置为0*/
{
    S->top = 0; /*把栈顶指针置为0*/
}
int StackEmpty(SeqStack S)
/*判断栈是否为空，栈为空返回1，否则返回0*/
{
    if (S.top == 0) {       /*判断栈顶指针top是否为0*/
        return 1;    /*当栈为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}
int GetTop(SeqStack S, DataType* e)
/*取栈顶元素。将栈顶元素值返回给e，并返回1表示成功；否则返回0表示失败。*/
{
    if (S.top <= 0) { /*在取栈顶元素之前，判断栈是否为空*/
        printf("栈已经空!\n");
        return 0;
    } else {
        *e = S.stack[S.top - 1]; /*在取栈顶元素*/
        return 1;
    }
}
int PushStack(SeqStack* S, DataType e)
/*将元素e进栈，元素进栈成功返回1，否则返回0.*/
{
    if (S->top >= StackSize) {          /*在元素进栈前，判断是否栈已经满*/
        printf("栈已满，不能进栈！\n");
        return 0;
    } else {
        S->stack[S->top] = e;       /*元素e进栈*/
        S->top++;                   /*修改栈顶指针*/
        return 1;
    }
}
int PopStack(SeqStack* S, DataType* e)
/*出栈操作。将栈顶元素出栈，并将其赋值给e。出栈成功返回1，否则返回0*/
{
    if (S->top <= 0) {  /*元素出栈之前，判断栈是否为空*/
        printf("栈已经没有元素，不能出栈!\n");
        return 0;
    } else {
        S->top--;           /*先修改栈顶指针，即出栈*/
        *e = S->stack[S->top];  /*将出栈元素赋值给e*/
        return 1;
    }
}
int StackLength(SeqStack S)
/*求栈的长度，即栈中元素个数，栈顶指针的值就等于栈中元素的个数*/
{
    return S.top;
}
