#include<stdio.h>
#include<math.h>
#define N 30
void Josephus();

void Josephus(int a[], int n, int s, int m)
/*约瑟夫问题，将出圈的人的序号依次存放在数组中*/
{
    int i, j, s1, t;
    s1 = s;             /*开始报数的序号*/

    for (i = 1; i <= n; i++) { /*初始化*/
        a[i - 1] = i;
    }

    for (i = n; i >= 2; i--) { /*i为当前圈中的人数*/
        s1 = (s1 + m - 1) % i; /*s1为将要出圈的序号*/

        if (s1 == 0) {  /*如果s1=0说明，将要出圈的人的序号为s1*/
            s1 = i;
        }

        /*将序号s1的人出圈，即将序号为s1数移到到数组的末尾*/
        t = a[s1 - 1];

        for (j = s1; j <= i - 1; j++) { /*将s1后的数依次前移*/
            a[j - 1] = a[j];
        }

        a[i - 1] = t; /*将序号为s1的数存储在a的末尾*/
    }
}

void main()
{
    int i, a[N];
    int n = 13, m = 3, s = 1;
    Josephus(a, n, s, m);
    printf("出圈的依次编号为:");

    for (i = n - 1; i >= 0; i--) {
        printf("%-4d", a[i]);
    }

    printf("\n");
}




