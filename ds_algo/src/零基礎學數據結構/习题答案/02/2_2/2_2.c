#include<stdio.h>
#include<math.h>
#define N 10
int a[N];
int Rd(int n, int k)
/*和式分解*/
{
    int j, c;

    for (c = n; c >= 1; c--) {      /*待分解的数是n*/
        if (c <= a[k -
                     1]) {        /*如果当前分解出的c小于等于上一个数（不增排列），则将c保存到数组中*/
            a[k] = c;

            if (c == n) {           /*如果c==n，则输出和式的分解*/
                printf("%d=%d", a[0], a[1]);

                for (j = 2; j <= k; j++) {
                    printf("+%d", a[j]);
                }

                printf("\n");
            } else {
                Rd(n - c, k + 1);    /*对数n-c进行分解，分解出第k+1个和数*/
            }
        }
    }

}

void main()
{
    int n = 4;
    a[0] = 4;
    Rd(n, 1);
}
