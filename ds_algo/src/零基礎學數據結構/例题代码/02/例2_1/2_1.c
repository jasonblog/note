#include<stdio.h>               /*包含输入和输出头文件*/
#define N 50
int sum(int a[], int n);        /*函数声明*/

void main()
{
    int i, n, a[N];
    printf("请输入一个50以内的自然数：");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {   /*把前n个自然数存放在数组a中*/
        a[i] = i + 1;
    }

    printf("前%d个自然数的和为：%d\n", n, sum(a, n));
}

int sum(int a[], int n)         /*函数实现*/
/*递归求数组a前n个数的和*/
{
    if (n <= 0) {               /*递归调用结束条件，当加上a[0]之后，退出递归调用*/
        return 0;
    } else {
        return a[n - 1] + sum(a, n - 1);    /*递归求前n个数的和*/
    }
}
