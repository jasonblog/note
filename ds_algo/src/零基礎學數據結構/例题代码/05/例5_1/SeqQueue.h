typedef struct Squeue { /*顺序队列类型定义*/
    DataType queue[QueueSize];
    int front, rear; /*队头指针和队尾指针*/
} SeqQueue;
void InitQueue(SeqQueue* SQ)
/*将顺序队列初始化为空队列只需要把队头指针和队尾指针同时置为0*/
{
    SQ->front = SQ->rear = 0; /*把队头指针和队尾指针同时置为0*/
}
int QueueEmpty(SeqQueue SQ)
/*判断队列是否为空，队列为空返回1，否则返回0*/
{
    if (SQ.front == SQ.rear) {   /*判断队头指针和队尾指针是否相等*/
        return 1;    /*当队列为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}
int EnterQueue(SeqQueue* SQ, DataType x)
/*将元素x插入到顺序队列SQ中，插入成功返回1，否则返回0*/
{
    if (SQ->rear ==
        QueueSize) { /*在插入新的元素之前，判断队尾指针是否到达数组的最大值，即是否队列已满*/
        return 0;
    }

    SQ->queue[SQ->rear] = x; /*在队尾插入元素x */
    SQ->rear = SQ->rear + 1; /*队尾指针向后移动一个位置*/
    return 1;
}
int DeleteQueue(SeqQueue* SQ, DataType* e)
/*删除顺序队列中的队头元素，并将该元素赋值给e，删除成功返回1，否则返回0*/
{
    if (SQ->front == SQ->rear) { /*在删除元素之前，判断队列是否为空*/
        return 0;
    } else {
        *e = SQ->queue[SQ->front];  /*将要删除的元素赋值给e*/
        SQ->front = SQ->front + 1; /*将队头指针向后移动一个位置，指向新的队头*/
        return 1;
    }
}
