//利用循环输出前40项
#include <stdio.h>
#include<string.h>
#include <stdio.h>
#define N 40
int Fib(int n);
int Fib2(int x)
/*fibonacci数列的非递归实现*/
{
    int n = 20;
    long f[N] = {0, 1};
    int i;

    for (i = 2; i < n; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    printf("非递归调用：fibonacci数列的前%d个数是：", n);

    for (i = 0; i < n - 1; i++) {
        printf("F%d==%d,", i + 1, f[i]);
    }

    printf("F%d==%d\n", i + 1, f[i]);
    return f[x - 1];
}


void main()
{
    int n, x;
    printf("请输入一个正整数n(0<n<20)\n");
    scanf("%d", &n);
    printf("递归调用：第%d个数是：%d\n", n, Fib(n));
    x = Fib2(n);
    printf("非递归调用：第%d个数是：%d\n", n, x);

}
int Fib(int n)
/*fibonacci数列的递归实现*/
{
    if (n == 1) {
        return 0;
    } else if (n == 2 || n == 3) {
        return 1;
    } else {
        return Fib(n - 1) + Fib(n - 2);
    }
}
