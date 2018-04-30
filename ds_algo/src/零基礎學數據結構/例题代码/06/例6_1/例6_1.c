#include<stdio.h>
#include"SeqString.h"
void StrPrint(SeqString S);
void main()
{
    SeqString S1, S2, Sub;
    char ch[MaxLength];
    printf("请输入第一个字符串:\n");
    gets(ch);
    StrAssign(&S1, ch);
    printf("输出串S1：");
    StrPrint(S1);
    printf("请输入第二个字符串:\n");
    gets(ch);
    StrAssign(&S2, ch);
    printf("输出串S2：");
    StrPrint(S2);
    printf("将串S2插入到S1的第13个位置:\n");
    StrInsert(&S1, 13, S2);
    StrPrint(S1);
    printf("将串S1中的第22个位置起的7个字符删除:\n");
    StrDelete(&S1, 22, 7);
    StrPrint(S1);
    printf("将串S2中的第6个位置起的4字符取出放入Sub中:\n");
    SubString(&Sub, S2, 6, 4);
    StrPrint(Sub);
    printf("将串Sub赋值为America:\n");
    StrAssign(&Sub, "America");
    printf("将串S1中的串S2用Sub取代:\n");
    StrReplace(&S1, S2, Sub);
    StrPrint(S1);
}
void StrPrint(SeqString S)
{
    int i;

    for (i = 0; i < S.length; i++) {
        printf("%c", S.str[i]);
    }

    printf("\n");
}
