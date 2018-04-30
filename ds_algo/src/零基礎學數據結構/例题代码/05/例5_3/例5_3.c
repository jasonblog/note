#include<stdio.h>       /*包含输出函数*/
#include<stdlib.h>      /*包含退出函数*/
#include<string.h>      /*包含字符串长度函数*/
#include<malloc.h>      /*包含内存分配函数*/
typedef char DataType;  /*类型定义为字符类型*/
/*链式堆栈结点类型定义*/
typedef struct snode {
    DataType data;
    struct snode* next;
} LSNode;
/*只有队尾指针的链式循环队列类型定义*/
typedef struct QNode {
    DataType data;
    struct QNode* next;
} LQNode, *LinkQueue;

void InitStack(LSNode** head)
/*带头结点的链式堆栈初始化*/
{
    if ((*head = (LSNode*)malloc(sizeof(LSNode))) == NULL) { /*为头结点分配空间*/
        printf("分配结点不成功");
        exit(-1);
    } else {
        (*head)->next = NULL;    /*头结点的指针域设置为空*/
    }
}
int StackEmpty(LSNode* head)
/*判断带头结点链式堆栈是否为空。如果堆栈为空，返回1，否则返回0*/
{
    if (head->next == NULL) { /*如果堆栈为空，返回1，否则返回0*/
        return 1;
    } else {
        return 0;
    }
}
int PushStack(LSNode* head, DataType e)
/*链式堆栈进栈。进栈成功返回1，否则退出*/
{
    LSNode* s;

    if ((s = (LSNode*)malloc(sizeof(LSNode))) ==
        NULL) { /*为结点分配空间，失败退出程序并返回-1*/
        exit(-1);
    } else {
        s->data = e;            /*把元素值赋值给结点的数据域*/
        s->next = head->next;   /*将结点插入到栈顶*/
        head->next = s;
        return 1;
    }
}
int PopStack(LSNode* head, DataType* e)
/*链式堆栈出栈，需要判断堆栈是否为空。出栈成功返回1，否则返回0*/
{
    LSNode* s = head->next;     /*指针s指向栈顶结点*/

    if (StackEmpty(head)) {     /*判断堆栈是否为空*/
        return 0;
    } else {
        head->next = s->next;   /*头结点的指针指向第二个结点位置*/
        *e = s->data;           /*要出栈的结点元素赋值给e*/
        free(s);                /*释放要出栈的结点空间*/
        return 1;
    }
}
void InitQueue(LinkQueue* rear)
/*将带头结点的链式循环队列初始化为空队列，需要把头结点的指针指向头结点*/
{
    if ((*rear = (LQNode*)malloc(sizeof(LQNode))) == NULL) {
        exit(-1);    /*如果申请结点空间失败退出*/
    } else {
        (*rear)->next = *rear;    /*队尾指针指向头结点*/
    }
}

int QueueEmpty(LinkQueue rear)
/*判断链式队列是否为空，队列为空返回1，否则返回0*/
{
    if (rear->next ==
        rear) {   /*判断队列是否为空。当队列为空时，返回1，否则返回0*/
        return 1;
    } else {
        return 0;
    }
}

int EnterQueue(LinkQueue* rear, DataType e)
/*将元素e插入到链式队列中，插入成功返回1*/
{
    LQNode* s;
    s = (LQNode*)malloc(sizeof(LQNode)); /*为将要入队的元素申请一个结点的空间*/

    if (!s) {
        exit(-1);    /*如果申请空间失败，则退出并返回参数-1*/
    }

    s->data = e;                        /*将元素值赋值给结点的数据域*/
    s->next = (*rear)->next;            /*将新结点插入链式队列*/
    (*rear)->next = s;
    *rear = s;                          /*修改队尾指针*/
    return 1;
}
int DeleteQueue(LinkQueue* rear, DataType* e)
/*删除链式队列中的队头元素，并将该元素赋值给e，删除成功返回1，否则返回0*/
{
    LQNode* f, *p;

    if (*rear ==
        (*rear)->next) { /*在删除队头元素即出队列之前，判断链式队列是否为空*/
        return 0;
    } else {
        f = (*rear)->next;      /*使指针f指向头结点*/
        p = f->next;            /*使指针p指向要删除的结点*/

        if (p == *rear) {       /*处理队列中只有一个结点的情况*/
            *rear = (*rear)->next; /*使指针rear指向头结点*/
            (*rear)->next = *rear;
        } else {
            f->next = p->next;    /*使头结点指向要出队列的下一个结点*/
        }

        *e = p->data;           /*把队头元素值赋值给e*/
        free(p);                /*释放指针p指向的结点*/
        return 1;
    }
}
void main()
{
    LinkQueue LQueue1, LQueue2;     /*定义链式循环队列*/
    LSNode* LStack1, *LStack2;      /*定义链式堆栈*/
    char str1[] = "XYZAZYX";        /*回文字符序列1*/
    char str2[] = "XYZBZXY";        /*回文字符序列2*/
    char q1, s1, q2, s2;
    int i;
    InitQueue(&LQueue1);                /*初始化链式循环队列1*/
    InitQueue(&LQueue2);                /*初始化链式循环队列2*/
    InitStack(&LStack1);                /*初始化链式堆栈1*/
    InitStack(&LStack2);                /*初始化链式堆栈2*/

    for (i = 0; i < strlen(str1); i++) {
        EnterQueue(&LQueue1, str1[i]);  /*依次把字符序列1入队*/
        EnterQueue(&LQueue2, str2[i]);  /*依次把字符序列2入队*/
        PushStack(LStack1, str1[i]);    /*依次把字符序列1进栈*/
        PushStack(LStack2, str2[i]);    /*依次把字符序列2进栈*/

    }

    printf("字符序列1：\n");
    printf("出队序列  出栈序列\n");

    while (!StackEmpty(LStack1)) {      /*判断堆栈1是否为空*/
        DeleteQueue(&LQueue1, &q1);     /*字符序列依次出队，并把出队元素赋值给q*/
        PopStack(LStack1, &s1);         /*字符序列出栈，并把出栈元素赋值给s*/
        printf("%5c", q1);              /*输出字符序列1*/
        printf("%10c\n", s1);

        if (q1 != s1) {                 /*判断字符序列1是否是回文*/
            printf("字符序列1不是回文！");
            return;
        }
    }

    printf("字符序列1是回文！\n");
    printf("字符序列2：\n");
    printf("出队序列  出栈序列\n");

    while (!StackEmpty(LStack2)) {      /*判断堆栈2是否为空*/
        DeleteQueue(&LQueue2, &q2);     /*字符序列依次出队，并把出队元素赋值给q*/
        PopStack(LStack2, &s2);         /*字符序列出栈，并把出栈元素赋值给s*/
        printf("%5c", q2);              /*输出字符序列2*/
        printf("%10c\n", s2);

        if (q2 != s2) {                 /*判断字符序列2是否是回文*/
            printf("字符序列2不是回文！\n");
            return;
        }
    }

    printf("字符序列2是回文！\n");
}
