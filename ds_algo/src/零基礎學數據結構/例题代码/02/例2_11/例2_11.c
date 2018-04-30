#include <stdio.h>      /*包含输入输出函数*/
int Max(int x, int y);  /*求两个整数的较大者的函数声明*/
void main()
{
    int a, b, s;
    scanf("%d,%d", &a, &b);
    s = Max(a, b);      /*调用求两个数中的较大者的函数*/
    printf("两个整数%d和%d的较大者为:%d\n", a, b, s);
}
int Max(int x, int y)
/*求两个整数较大者的实现函数，并将较大的值返回*/
{
    int z;

    if (x > y) {
        z = x;
    } else {
        z = y;
    }

    return z;
}
