#define QueueSize 20        /*定义顺序循环队列的最大容量*/
typedef int DataType;       /*定义顺序循环队列元素的类型为字符类型*/
#include<stdio.h>
typedef struct Squeue {         /*顺序循环队列的类型定义*/
    DataType sequ[QueueSize];
    int rear, quelen;       /*队尾指针、队中元素个数*/
} SCQueue;
void InitQueue(SCQueue* SCQ)
/*初始化顺序循环队列*/
{
    SCQ ->rear = 0; /*把队尾指针置为0*/
    SCQ->quelen = 0; /*初始时，队中元素个数为0*/
}
int QueueEmpty(SCQueue SCQ)
/*判断顺序循环队列是否为空，队列为空返回1，否则返回0*/
{
    if (SCQ.quelen == 0) {
        return 1;
    } else {
        return 0;
    }
}
int EnterQueue(SCQueue* SCQ, DataType e)
/*将元素e插入到顺序循环队列SCQ中，插入成功返回1，否则返回0*/
{
    if (SCQ->quelen == QueueSize - 1) {      /*判断队列是否已满*/
        return 0;
    }

    SCQ->sequ[SCQ->rear] = e;                /*在队尾插入元素e */
    SCQ->rear = (SCQ->rear + 1) % QueueSize; /*队尾指针向后移动一个位置*/
    SCQ->quelen = SCQ->quelen + 1;           /*修改队列元素的个数*/
    return 1;
}
int DeleteQueue(SCQueue* SCQ, DataType* e)
/*删除顺序循环队列中的队头元素，并将该元素赋值给e，删除成功返回1，否则返回0*/
{
    int front;

    if (SCQ->quelen == 0) { /*判断顺序循环队列是否为空*/
        return 0;
    } else {
        front = SCQ->rear - SCQ->quelen; /*求出队头元素所在的位置*/

        if (front < 0) {
            front = front + QueueSize;
        }

        *e = SCQ->sequ[front];          /*将队头的元素赋值给e*/
        SCQ->quelen = SCQ->quelen - 1;  /*修改队列中的元素个数*/
        return 1;
    }
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

    printf("出队列的元素依次是：");

    while (!QueueEmpty(Q)) {
        DeleteQueue(&Q, &e);
        printf("%4d", e);
    }

    printf("\n");
}

