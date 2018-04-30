/*包含头文件*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<stdarg.h>  /*标准头文件，包含va_start、va-arg、va_end宏定义*/
typedef int DataType;
#include"SeqArray.h"
void main()
{
    Array A;
    DataType B[4][3] = {{5, 6, 7}, {23, 45, 67}, {35, 2, 34}, {12, 36, 90}};
    int i, j;
    int dim = 2, bound1 = 4, bound2 = 3;    /*初始化数组的维数和各维的长度*/
    DataType e;
    InitArray(&A, dim, bound1, bound2);     /*构造一个4×3的二维数组A*/
    printf("数组A的各维的长度是:");

    for (i = 0; i < dim; i++) {             /*输出数组A的各维的长度*/
        printf("%3d", A.bounds[i]);
    }

    printf("\n数组A的常量基址是:");

    for (i = 0; i < dim; i++) {             /*输出数组A的常量基址*/
        printf("%3d", A.constants[i]);
    }

    printf("\n%d行%d列的矩阵元素如下:\n", bound1, bound2);

    for (i = 0; i < bound1; i++) {
        for (j = 0; j < bound2; j++) {
            AssignValue(A, B[i][j], i, j);  /*将数组B的元素赋值给A*/
            GetValue(&e, A, i, j);          /*将数组A中的元素赋值给e*/
            printf("A[%d][%d]=%3d\t", i, j, e); /*输出数组A中的元素*/
        }

        printf("\n");
    }

    printf("按照数组的线性序列输出元素,即利用基地址输出元素:\n");

    for (i = 0; i < bound1 * bound2; i++) { /*按照线性序列输出数组A中的元素*/
        printf("第%d个元素=%3d\t", i + 1, A.base[i]);

        if ((i + 1) % bound2 == 0) {
            printf("\n");
        }
    }

    DestroyArray(&A);
}

