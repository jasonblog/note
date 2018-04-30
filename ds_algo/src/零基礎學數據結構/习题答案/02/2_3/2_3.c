#include<stdio.h>
#include<math.h>

int Distribute(int n)
/*输出整数的各位数字*/
{
    int j, c;

    if (n >= 10) {      /*如果n的位数大于两位数，则执行n/10。输出n的各位数字*/
        Distribute(n / 10);
    }

    printf("%3d", n % 10); /*输出n的最后一位数字*/

}

void main()
{
    int n = 4567;
    printf("整数%d的各位数字是:", n);
    Distribute(n);
    printf("\n");
}