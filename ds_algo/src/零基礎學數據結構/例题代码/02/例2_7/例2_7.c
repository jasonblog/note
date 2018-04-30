#include<stdio.h>                       /*包含输入输出*/
int Sum(int a, int b);                  /*求和函数声明*/
void main()
{
    int a, b;
    int (*fun)(int, int);               /*声明一个函数指针*/
    printf("请输入两个数:");
    scanf("%d,%d", &a, &b);
    /*第一种调用函数的方法：函数名调用求和函数*/
    printf("第一种调用函数的方法：函数名调用求和函数:\n");
    printf("%d+%d=%d\n", a, b, Sum(a, b)); /*通过函数名调用*/
    /*第二种调用函数的方法：函数指针调用求和函数*/
    fun = Sum;                          /*函数指针指向求和函数*/
    printf("第二种调用函数的方法：函数指针调用求和函数:\n");
    printf("%d+%d=%d\n", a, b, (*fun)(a, b)); /*通过函数指针调用函数*/
}
int Sum(int m, int n)                   /*求和函数实现*/
{
    return m + n;
}
