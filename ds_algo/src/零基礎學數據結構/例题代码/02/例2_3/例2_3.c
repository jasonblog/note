#include<stdio.h>           /*包含输入输出*/
void main()
{
    int a = 10, b = 20;
    int* aPtr, *bPtr;               /*指针变量声明*/
    aPtr = &a;              /*指针变量指向变量q*/
    bPtr = &b;
    /*打印变量q的地址和qPtr的内容*/
    printf("q的地址是：%p\nqPtr中的内容是：%p\n", &q, qPtr);
    /*打印q的值和qPtr指向变量的内容*/
    printf("q的值是：%d\n*qPtr的值是：%d\n", q, *qPtr);
    /*运算符'&'和'*'是互逆的*/
    printf("&*qPtr=%p,*&qPtr=%p\n因此有&*qPtr=*&qPtr\n", &*qPtr, *& qPtr);
}

