#include<stdio.h>                       /*包含输入输出*/
void main()
{
    int a[5] = {10, 20, 30, 40, 50};
    int* aPtr, i;                       /*指针变量声明*/
    aPtr = &a[0];                       /*指针变量指向变量q*/

    for (i = 0; i < 5; i++) {                   /*通过数组下标引用元素*/
        printf("a[%d]=%d\n", i, a[i]);
    }

    for (i = 0; i < 5; i++) {                   /*通过数组名引用元素*/
        printf("*(a+%d)=%d\n", i, *(a + i));
    }

    for (i = 0; i < 5; i++) {                   /*通过指针变量下标引用元素*/
        printf("aPtr[%d]=%d\n", i, aPtr[i]);
    }

    for (aPtr = a, i = 0; aPtr < a + 5; aPtr++, i++) { /*通过指针变量偏移引用元素*/
        printf("*(aPtr+%d)=%d\n", i, *aPtr);
    }
}

