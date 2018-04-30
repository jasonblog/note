#define QueueSize 20        /*定义顺序循环队列的最大容量*/
typedef int DataType;       /*定义顺序循环队列元素的类型为字符类型*/
#include<stdio.h>
typedef struct Squeue {         /*顺序循环队列的类型定义*/
    DataType queue[QueueSize];
    int front, rear;        /*队头指针和队尾指针*/
} SCQueue;
void InitQueue(SCQueue* SCQ)
/*将顺序循环队列初始化为空队列只需要把队头指针和队尾指针同时置为0*/
{
    SCQ ->rear = 0;
    SCQ->front = 0;
}
int QueueEmpty(SCQueue SCQ)
/*判断顺序循环队列是否为空，队列为空返回1，否则返回0*/
{
    if (SCQ.front == SCQ.rear) { /*当顺序循环队列为空时，返回1，否则返回0*/
        return 1;
    } else {
        return 0;
    }
}

int EnterQueue(SCQueue* SCQ, DataType e)
/*将元素e插入到顺序循环队列SCQ中，插入成功返回1，否则返回0*/
{
    if (SCQ->front == (SCQ->rear + 1) % QueueSize) {
        return 0;
    }

    SCQ->queue[SCQ->rear] = e;               /*在队尾插入元素e */
    SCQ->rear = (SCQ->rear + 1) % QueueSize; /*移动队尾指针*/
    return 1;
}

int DeleteQueue(SCQueue* SCQ, DataType* e)
/*删除顺序循环队列中的队头元素，并将该元素赋值给e，删除成功返回1，否则返回0*/
{
    if (SCQ->front ==
        SCQ->rear) {              /*在删除元素之前，判断顺序循环队列是否为空*/
        return 0;
    } else {
        *e = SCQ->queue[SCQ->front];            /*将要删除的元素赋值给e*/
        SCQ->front = (SCQ->front + 1) % QueueSize; /*移动队头指针*/
        return 1;
    }
}

/*------------求环形队列中元素的个数-------------------*/
int ComputeCount(SCQueue SCQ)
{
    int count;
    count = (SCQ.rear - SCQ.front + QueueSize) % QueueSize;
    return count;
}
void main()
{
    SCQueue Q;
    int i, n = 10;
    DataType a[] = {34, 3, 56, 43, 21, 51, 46, 89, 98, 76}, e;
    InitQueue(&Q);

    for (i = 0; i < n; i++) {
        EnterQueue(&Q, a[i]);
    }

    printf("队列中的元素个数是：%2d\n", ComputeCount(Q));
    printf("出队列的元素依次是：");

    for (i = 0; i < n; i++) {
        DeleteQueue(&Q, &e);
        printf("%4d", e);
    }

    printf("\n");
    printf("清空队列后，队列中的元素个数是：%2d\n", ComputeCount(Q));
}

