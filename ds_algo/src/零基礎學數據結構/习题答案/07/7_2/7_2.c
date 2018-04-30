#include<stdio.h>
#define N 20

void PrintMatrix(int a[N][N], int n)
/*打印螺旋矩阵*/
{
    int i, j, k, m;
    k = 1;

    for (i = 0; i < n / 2; i++) {
        for (j = i; j < n - i; j++) { /*打印上面的行*/
            a[i][j] = k++;
        }

        for (j = i + 1; j < n - i; j++) { /*打印右边的列*/
            a[j][n - i - 1] = k++;
        }

        for (j = n - i - 2; j >= i; j--) { /*打印下面的行*/
            a[n - i - 1][j] = k++;
        }

        for (j = n - i - 2; j >= i + 1; j--) { /*打印左边的列*/
            a[j][i] = k++;
        }
    }

    if (n % 2 != 0) {           /*如果n是奇数，则将最后一个数存入*/
        a[n / 2][n / 2] = k;
    }
}
void main()
{
    int a[N][N], n = 5, i, j;
    PrintMatrix(a, n);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%3d", a[i][j]);
        }

        printf("\n");
    }

}