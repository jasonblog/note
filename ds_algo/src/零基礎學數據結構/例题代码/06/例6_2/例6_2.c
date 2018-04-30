/*包含头文件和串的实现文件*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"HeapString.h"
void StrPrint(HeapString S);    /*串的输出函数声明*/
void main()
{
    HeapString S1, S2, Sub;
    char* p;
    char ch[50];
    /*初始化串S1，S2和Sub*/
    InitString(&S1);
    InitString(&S2);
    InitString(&Sub);
    printf("请输入第一个字符串：");
    gets(ch);
    StrAssign(&S1, ch);             /*通过输入将串S1赋值*/
    printf("经过赋值操作后的串S1：\n");
    StrPrint(S1);
    printf("请输入第二个字符串：");
    gets(ch);
    StrAssign(&S2, ch);             /*通过输入将串S2赋值*/
    printf("经过赋值操作后的串S2：\n");
    StrPrint(S2);
    printf("把串S2连接在串S1的末尾，S1串为：\n");
    StrCat(&S1, S2);                /*将串S2连接在串S1的末尾*/
    StrPrint(S1);
    printf("经过赋值操作后的串Sub：\n");
    StrAssign(&Sub, "Everyone");    /*将串Sub赋值*/
    StrPrint(Sub);
    printf("将串S2插入到串S1的第一位置：\n");
    StrInsert(&S1, 1, Sub);         /*将串Sub插入到串S1的第一位置*/
    StrPrint(S1);
    printf("把串S1的第1个位置之后的8个字符删除：\n");
    StrDelete(&S1, 1, 8);           /*将串S1的第一位置后的8个字符删除*/
    StrPrint(S1);
    printf("将串S1中的S2置换为Sub：\n");
    StrAssign(&Sub, "Xi'an");       /*将串Sub重新赋值*/
    StrReplace(&S1, S2, Sub);       /*用串Sub取代串S1中的S2*/
    StrPrint(S1);
    /*将串S1，S2和Sub的内存单元释放*/
    StrDestroy(&S1);
    StrDestroy(&S2);
    StrDestroy(&Sub);
}
void StrPrint(HeapString S)
{
    int i;

    for (i = 0; i < S.length; i++) {
        printf("%c", S.str[i]);
    }

    printf("\n");
}
