
typedef struct QNode {
    DataType data;
    struct QNode* next;
} LQNode, *QueuePtr;
typedef struct {
    QueuePtr front;
    QueuePtr rear;
} LinkQueue;


void InitQueue(LinkQueue* LQ)
/*将带头结点的链式队列初始化为空队列需要把头结点的指针域置为0*/
{
    LQ->front = (LQNode*)malloc(sizeof(LQNode));

    if (LQ->front == NULL) {
        exit(-1);
    }

    LQ ->front->next = NULL; /*把头结点的指针域置为为0*/
    LQ->rear = LQ->front;
}
int QueueEmpty(LinkQueue LQ)
/*判断链式队列是否为空，队列为空返回1，否则返回0*/
{
    if (LQ.front->next == NULL) {   /*当链式队列为空时，返回1，否则返回0*/
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
    s->next = NULL;                     /*将结点的指针域置为空*/
    LQ->rear->next = s;                 /*将原来队列的队尾指针指向p*/
    LQ->rear = s;                   /*将队尾指针指向p*/
    return 1;
}
//int DeleteQueue(LinkQueue *LQ,DataType *e)
///*删除链式队列中的队头元素，并将该元素赋值给e，删除成功返回1，否则返回0*/
//{
//    LQNode *s;
//if(LQ->front==LQ->rear)       /*在删除元素之前，判断链式队列是否为空*/
//        return 0;
//    else
//{
//    s=LQ->front->next;                /*使指针p指向队头元素的指针*/
//    *e=s->data;                       /*将要删除的队头元素赋值给e*/
//    LQ->front->next=s->next;          /*使头结点的指针指向指针p的下一个结点*/
//    if(LQ->rear==s) LQ->rear=LQ->front;   /*如果要删除的结点是队尾，则使队尾指针指向队头指针*/
//    free(s);                      /*释放指针p指向的结点*/
//        return 1;
//    }
//}
int GetHead(LinkQueue LQ, DataType* e)
/*取链式队列中的队头元素，并将该元素赋值给e，取元素成功返回1，否则返回0*/
{
    LQNode* s;

    if (LQ.front == LQ.rear) {  /*在取队头元素之前，判断链式队列是否为空*/
        return 0;
    } else {
        s = LQ.front->next; /*将指针p指向队列的第一个元素即队头元素*/
        *e = s->data;       /*将队头元素赋值给e，取出队头元素*/
        return 1;
    }
}
/*出队操作。*/
int DeleteQueue(LinkQueue* Q, DataType* x)
{
    /* 将队列Q的队头元素出队，并存放到x所指的存储空间中 */
    LQNode* p;

    if (Q->front == Q->rear) {
        return (0);
    }

    p = Q->front->next;
    Q->front->next = p->next; /* 队头元素p出队 */

    if (Q->front == NULL) { /* 如果队中只有一个元素p，则p出队后成为空队 */
        Q->rear = Q->front;
    }

    *x = p->data;
    free(p);   /* 释放存储空间 */
    return (1);
}