#include<stdio.h>           /*包含输出函数*/
#define QueueSize 8         /*定义双端队列的大小*/
typedef char DataType;      /*定义数据类型为字符类型*/
typedef struct DQueue {     /*双端队列的类型定义*/
    DataType queue[QueueSize];
    int end1, end2;         /*双端队列的队尾指针*/
} DQueue;
int EnterQueue(DQueue* DQ, DataType e, int tag)
/*将元素e插入到双端队列中。如果成功返回1，否则返回0*/
{
    switch (tag) {
    case 1:                     /*1表示在队列的左端入队*/
        if (DQ->end1 != DQ->end2) { /*元素入队之前判断队列是否为满*/
            DQ->queue[DQ->end1] = e;        /*元素e入队*/
            DQ->end1 = (DQ->end1 - 1) % QueueSize; /*向左移动队列指针*/
            return 1;
        } else {
            return 0;
        }

    case 2:                     /*1表示在队列的左端入队*/
        if (DQ->end1 != DQ->end2) { /*元素入队之前判断队列是否已满*/
            DQ->queue[DQ->end2] = e;        /*元素e入队*/
            DQ->end2 = (DQ->end2 + 1) % QueueSize; /*向右移动队列指针*/
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}
int DeleteQueue(DQueue* DQ, DataType* e, int tag)
/*将元素出队列，并将出队列的元素赋值给e。如果出队列成功返回1，否则返回0*/
{
    switch (tag) {
    case 1:                     /*1表示在队列的左端出队*/
        if (((DQ->end1 + 1) % QueueSize) !=
            DQ->end2) { /*在元素出队列之前判断队列是否为空*/
            DQ->end1 = (DQ->end1 + 1) % QueueSize; /*向右移动队列指针，将元素出队列*/
            *e = DQ->queue[DQ->end1];       /*将出队列的元素赋值给e*/
            return 1;
        } else {
            return 0;
        }

    case 2:
        if (((DQ->end2 - 1) % QueueSize) !=
            DQ->end1) { /*在元素出队列之前判断队列是否为空*/
            DQ->end2 = (DQ->end2 - 1) % QueueSize; /*向左移动队列指针，将元素出队列*/
            *e = DQ->queue[DQ->end2];       /*将出队列的元素赋值给e*/
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}
void main()
{
    DQueue Q;           /*定义双端队列Q*/
    char ch;            /*定义字符*/
    Q.end1 = 3;         /*设置队列的初始状态*/
    Q.end2 = 4;

    /*将元素a，b，c在队列左端入队*/
    if (!EnterQueue(&Q, 'a', 1)) {      /*元素左端入队*/
        printf("队列已满，不能入队！");
    } else {
        printf("a左端入队：\n");
    }

    if (!EnterQueue(&Q, 'b', 1)) {
        printf("队列已满，不能入队！");
    } else {
        printf("b左端入队：\n");
    }

    if (!EnterQueue(&Q, 'c', 1)) {
        printf("队列已满，不能入队！");
    } else {
        printf("c左端入队：\n");
    }

    /*将元素d，e在队列右端入队*/
    if (!EnterQueue(&Q, 'd', 2)) {      /*元素右端入队*/
        printf("队列已满，不能入队！");
    } else {
        printf("d右端入队：\n");
    }

    if (!EnterQueue(&Q, 'e', 2)) {
        printf("队列已满，不能入队！");
    } else {
        printf("e右端入队：\n");
    }

    /*元素c，b，e，d依次出队列*/
    printf("队列左端出队一次：");
    DeleteQueue(&Q, &ch, 1);            /*元素左端出队列*/
    printf("%c\n", ch);
    printf("队列左端出队一次：");
    DeleteQueue(&Q, &ch, 1);
    printf("%c\n", ch);
    printf("队列右端出队一次：");
    DeleteQueue(&Q, &ch, 2);            /*元素右端出队列*/
    printf("%c\n", ch);
    printf("队列右端出队一次：");
    DeleteQueue(&Q, &ch, 2);
    printf("%c\n", ch);
}
