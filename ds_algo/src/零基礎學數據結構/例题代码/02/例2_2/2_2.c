#include<stdio.h>           /*包含输入和输出头文件*/
#define N 50
int NonRecSum(int a[], int n);          /*函数声明*/
void main()
{
    int i, n, a[N];
    printf("请输入一个50以内的自然数：");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {   /*把前n个自然数存放在数组a中*/
        a[i] = i + 1;
    }

    printf("前%d个自然数的和为：%d\n", n, NonRecSum(a, n));
}
int NonRecSum(int a[], int n)
/*非递归求数组a前n个数的和*/
{
    int i, sum = 0;

    for (i = 0; i < n; i++) {   /*通过迭代求n个自然数的和*/
        sum += i;
    }

    return sum;
}
