#include<stdio.h>   /*包含输入输出头文件*/
void f1();          /*函数f1声明*/
void f2();          /*函数f2声明*/
void f3();          /*函数f3声明*/
void main()
{
    void (*f[3])() = {f1, f2, f3};      /*指向函数的指针数组的声明*/
    int flag;
    printf("请输入一个1,2,或者3. 输入0退出.\n");
    scanf("%d", &flag);

    while (flag) {
        if (flag == 1 || flag == 2 || flag == 3) {
            f[flag - 1](flag - 1);      /*通过函数指针调用数组中的函数*/
            printf("请输入一个1,2,或者3.输入0退出.\n");
            scanf("%d", &flag);
        } else {
            printf("请输入一个合法的数(1-3),0退出.\n");
            scanf("%d", &flag);
        }
    }

    printf("程序退出.\n");
}
void f1()                       /*函数f1的定义*/
{
    printf("函数f1被调用！\n"); /*函数f2的定义*/
}
void f2()
{
    printf("函数f2被调用！\n"); /*函数f3的定义*/
}
void f3()
{
    printf("函数f3被调用！\n");
}
