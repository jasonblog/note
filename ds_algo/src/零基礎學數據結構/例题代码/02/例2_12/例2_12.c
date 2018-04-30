#include <stdio.h>          /*包含输入输出函数*/
void Swap(int* x, int* y);  /*交换两个数的函数声明*/
void main()
{
    int a, b;
    printf("请输入两个整数：\n");
    scanf("%d,%d", &a, &b);

    if (a < b) {
        Swap(&a, &b);    /*两个数中如果前者小，则交换两个值，使其较大的保存在a中较小保存在b中*/
    }

    printf("两个整数%d和%d的较大者为:%d,较小者为:%d\n", a, b, a, b);
}
void Swap(int* x, int* y)
/*交换两个数，较大的一个保存在*x中，较小的一个保存在*y*/
{
    int z;
    z = *x;
    *x = *y;
    *y = z;
}
