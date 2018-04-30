#include<stdio.h>                       /*包含输入输出*/
void main()
{
    /*指针数组定义*/
    char* book[4] = {"C Programming Language", "Assembly Language", "Java Language", "Natural Language"};
    int n = 4;                      /*指针数组元素的个数*/
    int row;
    char* arrayPtr;
    /*第一种方法输出：通过数组名输出*/
    printf("第一种方法输出：通过指针数组中的各个数组名输出:\n");

    for (row = 0; row < n; row++) {
        printf("第%d个字符串：%s\n", row + 1, book[row]);
    }

    /*第二种方法输出：通过指向数组的指针输出*/
    printf("第二种方法输出：通过指向各个数组的指针输出:\n");

    for (arrayPtr = book[0], row = 0; row < n; arrayPtr = book[row]) {
        printf("第%d个字符串：%s\n", row + 1, arrayPtr);
        row++;
    }
}

