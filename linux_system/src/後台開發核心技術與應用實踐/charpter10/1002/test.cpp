#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int global = 1; /*初始化的全局变量，存在data段*/
int main(void)
{
    pid_t pid;/*存储进程id*/
    int   stack = 1;/*局部变量，存在栈中*/
    int*  heap;/*指向堆变量的指针*/
    heap = (int*)malloc(sizeof(int));
    *heap = 3;/*设置堆中的值是3*/
    pid = fork();/*创建一个新的进程*/

    if (pid < 0) {
        perror("fail to fork");
        exit(-1);
    } else if (pid == 0) {
        /*子进程，改变变量的值*/
        global++;
        stack++;
        (*heap)++;
        /*打印出变量的值*/
        printf("In sub-process, global: %d, stack: %d, heap: %d\n", global, stack,
               *heap);
        exit(0);
    } else {
        /*父进程*/
        sleep(2);/*休眠2秒钟，确保子进程已执行完毕，再执行父进程*/
        printf("In parent-process, global: %d, stack: %d, heap: %d\n", global, stack,
               *heap);
    }

    return 0;
}
