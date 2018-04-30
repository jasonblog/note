/*包含头文件及链串的基本操作实现文件*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"LinkString.h"
void StrPrint(LinkString S);
void main()
{
    int i, j;
    int flag;
    LinkString S1, S2, S3, Sub;
    char* str1 = "Welcome to";
    char* str2 = " Data Structure";
    char* str3 = "Computer Architecture";
    printf("串的初始化和赋值操作:\n");
    InitString(&S1);                    /*串S1，S2，S3的初始化*/
    InitString(&S2);
    InitString(&S3);
    InitString(&Sub);
    StrAssign(&S1, str1);               /*串S1，S2，S3的赋值操作*/
    StrAssign(&S2, str2);
    StrAssign(&S3, str3);
    printf("串S1的值是:");
    StrPrint(S1);
    printf("串S2的值是:");
    StrPrint(S2);
    printf("串S3的值是:");
    StrPrint(S3);
    printf("将串S2连接在串S1的末尾:\n");
    StrConcat(&S1, S2);                 /*将串S2连接在串S1的末尾*/
    printf("S1是:");
    StrPrint(S1);
    printf("将串S1的第12个位置后的14个字符删除:\n");
    StrDelete(&S1, 12, 14);             /*将串S1中的第12个位置后的14个字符删除*/
    printf("S1是:");
    StrPrint(S1);
    printf("将串S2插入到串S1中的第12个字符后:\n");
    StrInsert(&S1, 12, S3);             /*将串S3插入到串S1的第12个字符后*/
    printf("S1是:");
    StrPrint(S1);
    printf("将串S1中的第12个字符后的8个字符取出并赋值给串Sub:\n");
    SubString(&Sub, S1, 12,
              8);         /*将串S1中的第12个位置后的8个字符取出赋值给Sub*/
    printf("Sub是:");
    StrPrint(Sub);
}

void StrPrint(LinkString S)
/*链串的输出*/
{
    int i = 0, j;
    Chunk* h;
    h = S.head;                     /*h指向第一个结点*/

    while (i < S.length) {
        for (j = 0; j < ChunkSize; j++) /*输出块中的每一个字符*/
            if (*(h->ch + j) != stuff) {
                printf("%c", *(h->ch + j));
                i++;
            }

        h = h->next;            /*h指向下一个结点*/
    }

    printf("\n");
}
