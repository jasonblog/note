#define QueueSize 50    /*定义队列的最大容量*/
typedef char DataType;  /*定义队列元素的类型为字符类型*/
#include<stdio.h>       /*包含头文件，主要包含输入输出函数*/
#include"SeqQueue.h"    /*包含顺序队列的实现算法文件*/

void main()
{
    SeqQueue Q;
    char str[] = "ABCDEFGH"; /*定义将要插入队列的字符串*/
    int i, length = 8;      /*定义队列的元素个数*/
    char x;
    InitQueue(&Q);          /*初始化顺序队列*/

    for (i = 0; i < length; i++) {
        EnterQueue(&Q, str[i]); /*将字符依次插入到顺序队列中*/
    }

    DeleteQueue(&Q, &x);    /*将队头元素出队列*/
    printf("出队列的元素为:%c\n", x);   /*显示输出出队列的元素*/
    printf("顺序队列中的元素为:");

    if (!QueueEmpty(Q)) {           /*判断队列是否为空队列*/
        for (i = Q.front; i < Q.rear;
             i++) { /*输出队头指针到队尾指针之间的元素，即队列所有元素*/
            printf("%c", Q.queue[i]);
        }

    }

    printf("\n");
}
