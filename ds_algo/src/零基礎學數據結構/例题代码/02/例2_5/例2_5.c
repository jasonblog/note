#include<stdio.h>                       /*包含输入输出*/
void main()
{
    int a[3][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
    int (*p)[4];                        /*数组指针指针变量声明*/
    int row, col;
    p = a;                              /*指针p指向数组元素为4的数组*/

    /*打印输出数组指针p指向的数组的值*/
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 4; col++) {
            printf("a[%d,%d]=%-4d", row, col, *(*(p + row) + col));
        }

        printf("\n");
    }

    /*通过改变指针p修改数组a的行地址，改变col的值修改数组a的列地址*/
    for (p = a, row = 0; p < a + 3; p++, row++) {
        for (col = 0; col < 4; col++) {
            printf("(*p[%d])[%d]=%p", row, col, ((*p) + col));
        }

        printf("\n");
    }
}

