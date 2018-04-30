
typedef struct {
    DataType stack[StackSize];
    int top[2];
} SSeqStack;


void InitStack(SSeqStack* S)
/*共享栈的初始化操作*/
{
    S->top[0] = 0;
    S->top[1] = StackSize - 1;
}
int PushStack(SSeqStack* S, DataType e, int flag)
/*共享栈进栈操作。进栈成功返回1，否则返回0*/
{
    if (S->top[0] == S->top[1]) {   /*在进栈操作之前，判断共享栈是否为空*/
        return 0;
    }

    switch (flag) {
    case 0:             /*当flag为0，表示元素要进左端的栈*/
        S->stack[S->top[0]] = e; /*元素进栈*/
        S->top[0]++;            /*修改栈顶指针*/
        break;

    case 1:             /*当flag为1，表示元素要进右端的栈*/
        S->stack[S->top[1]] = e; /*元素进栈*/
        S->top[1]--;            /*修改栈顶指针*/
        break;

    default:
        return 0;
    }

    return 1;
}
int PopStack(SSeqStack* S, DataType* e, int flag)
{
    switch (flag) {         /*在出栈操作之前，判断是哪个栈要进行出栈操作*/
    case 0:
        if (S->top[0] == 0) { /*左端的栈为空，则返回0，表示出栈操作失败*/
            return 0;
        }

        S->top[0]--;        /*修改栈顶指针*/
        *e = S->stack[S->top[0]]; /*将出栈的元素赋值给e*/
        break;

    case 1:
        if (S->top[1] == StackSize - 1) { /*右端的栈为空，则返回0，表示出栈操作失败*/
            return 0;
        }

        S->top[1]++;                /*修改栈顶指针*/
        *e = S->stack[S->top[1]]; /*将出栈的元素赋值给e*/
        break;

    default:
        return 0;
    }

    return 1;
}
