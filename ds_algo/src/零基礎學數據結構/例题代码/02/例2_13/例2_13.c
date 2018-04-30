#include <stdio.h>          /*包含输入输出函数*/
#define N 10
void MulArray1(int* x, int n);  /*数组名作为参数的函数原型*/
void MulArray2(int* aPtr, int n); /*指针作为参数的函数原型*/
void main()
{
    int a[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    printf("原来数组中的元素为:\n");

    for (i = 0; i < N; i++) {
        printf("%4d", a[i]);
    }

    printf("\n");
    printf("数组元素第一次放大5倍后为:\n");
    MulArray1(a, N);            /*调用数组名作为参数的函数*/

    for (i = 0; i < N; i++) {
        printf("%4d", a[i]);
    }

    printf("\n");
    printf("数组元素第二次放大5倍后为:\n");
    MulArray2(a, N);            /*调用指针作为参数的函数*/

    for (i = 0; i < N; i++) {
        printf("%4d", a[i]);
    }

    printf("\n");
}
void MulArray1(int b[], int n)
/*数组名作为参数的实现函数。*/
{
    int i;

    for (i = 0; i < n; i++) {
        b[i] = b[i] * 5;
    }
}
void MulArray2(int* aPtr, int n)
/*指针作为参数的实现。通过指针访问每一个元素。*/
{
    int i;

    for (i = 0; i < n; i++) {
        *(aPtr + i) = *(aPtr + i) * 5;
    }
}