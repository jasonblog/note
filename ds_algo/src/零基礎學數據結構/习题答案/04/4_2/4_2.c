#include<stdio.h>
#include<malloc.h>
typedef char DataType;
#define MaxSize 100
typedef struct Node {
    DataType data;
    struct Node* next;
} LStackNode, *LinkStack;
void InitStack(LinkStack* top)
/*将链栈初始化为空。动态生成头结点，并将头结点的指针域置为空。*/
{
    if ((*top = (LinkStack)malloc(sizeof(LStackNode))) ==
        NULL) { /*为头结点分配一个存储空间*/
        exit(-1);
    }

    (*top)->next = NULL;            /*将链栈的头结点指针域置为空*/
}

int PushStack(LinkStack top, DataType e)
/*进栈操作就是要在链表的第一个结点前插入一个新结点，进栈成功返回1*/
{
    LStackNode*
    p;      /*定义指向第i个元素的前驱结点指针pre，指针p指向新生成的结点*/

    if ((p = (LStackNode*)malloc(sizeof(LStackNode))) == NULL) {
        printf("内存分配失败!");
        exit(-1);
    }

    p->data = e;        /*指针p指向头结点*/
    p->next = top->next;
    top->next = p;
    return 1;
}
int PopStack(LinkStack top, DataType* e)
/*删除单链表中的第i个位置的结点。删除成功返回1，失败返回0*/
{
    LStackNode* p;
    p = top->next;

    if (!p) {                       /*判断链栈是否为空*/
        printf("栈已空");
        return 0;
    }

    top->next = p->next;            /*将栈顶结点与链表断开，即出栈*/
    *e = p->data;                   /*将出栈元素赋值给e*/
    free(p);                        /*释放p指向的结点*/
    return 1;
}

void DestroyStack(LinkStack top)
/*销毁栈*/
{
    LStackNode* p, *q;
    p = top;

    while (!p) {
        q = p;
        p = p->next;
        free(q);
    }
}
int GetTop(LinkStack top, DataType* e)
/*取栈顶元素*/
{
    LStackNode* p;
    p = top->next;

    if (!p) {                       /*判断链栈是否为空*/
        printf("栈已空");
        return 0;
    }

    *e = p->data;               /*将出栈元素赋值给e*/
    return 1;
}
void main()
{
    char str[MaxSize];
    DataType e;
    LinkStack S;
    int i;
    InitStack(&S);
    printf("请输入字符串：");
    gets(str);

    for (i = 0; i < strlen(str); i++) {
        PushStack(S, str[i]);
    }

    printf("栈顶元素是：");
    GetTop(S, &e);
    printf("%c\n", e);
    printf("进行出栈操作，出栈的元素依次是：");

    for (i = 0; i < strlen(str); i++) {
        PopStack(S, &e);
        printf("%4c", e);
    }

    printf("\n");
    DestroyStack(&S);

}