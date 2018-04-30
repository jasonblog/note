#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
typedef int DataType;
typedef struct QNode { /*队列中结点定义*/
    DataType data;
    struct QNode* next;
} LQNode, *QueuePtr;
typedef struct { /*链式队列的定义*/
    QueuePtr rear;/*rear指向队列中队尾结点*/
} LinkQueue;


void InitQueue(LinkQueue* LQ)
/*链式循环队列的初始化*/
{
    if ((LQ->rear = (LQNode*)malloc(sizeof(LQNode))) ==
        NULL) { /*如果申请结点空间失败退出*/
        exit(-1);
    } else {
        LQ->rear->next = LQ->rear;    /*队尾指针指向头结点*/
    }
}
int QueueEmpty(LinkQueue LQ)
/*判断链式队列是否为空，队列为空返回1，否则返回0*/
{
    if (LQ.rear == LQ.rear->next) { /*当链式队列为空时，返回1，否则返回0*/
        return 1;
    } else {
        return 0;
    }
}
int EnterQueue(LinkQueue* LQ, DataType e)
/*将元素e插入到链式队列LQ中，插入成功返回1*/
{
    LQNode* s;
    s = (LQNode*)malloc(sizeof(LQNode)); /*为将要入队的元素申请一个结点的空间*/

    if (!s) {
        exit(-1);    /*如果申请空间失败，则退出并返回参数-1*/
    }

    s->data = e;                        /*将元素值赋值给结点的数据域*/
    s->next = LQ->rear->next;           /*将结点*s插入到队列中*/
    LQ->rear->next = s;
    LQ->rear = s;                       /*使s称为新的队尾*/
    return 1;
}

int DeleteQueue(LinkQueue* LQ, DataType* e)
/*出队操作*/
{
    LQNode* f, *p;

    if (LQ->rear ==
        LQ->rear->next) { /*在删除队头元素即出队列之前，判断链式队列是否为空*/
        return 0;
    } else {
        f = LQ->rear->next;         /*使指针f指向头结点*/
        p = f->next;                /*使指针p指向要删除的结点*/

        if (p == LQ->rear) {        /*处理队列中只有一个结点的情况*/
            LQ->rear = LQ->rear->next;  /*使指针rear指向头结点*/
            LQ->rear->next = LQ->rear;
        } else {
            f->next = p->next;    /*使头结点指向要出队列的下一个结点*/
        }

        *e = p->data;               /*把队头元素值赋值给e*/
        free(p);                    /*释放指针p指向的结点*/
        return 1;
    }
}

void main()
{
    LinkQueue Q;
    DataType a[] = {3, 5, 6, 7, 9, 12}, e;
    int n = 6, i;
    InitQueue(&Q);

    for (i = 0; i < n; i++) {
        EnterQueue(&Q, a[i]);
    }

    printf("依次删除队列中的元素：");

    while (!QueueEmpty(Q)) {
        DeleteQueue(&Q, &e);
        printf("%4d", e);
    }

    printf("\n");

}
