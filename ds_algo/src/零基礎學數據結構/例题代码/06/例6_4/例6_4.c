/*包含头文件*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"SeqString.h"
/*函数的声明*/
int B_FIndex(SeqString S, int pos, SeqString T, int* count);
int KMP_Index(SeqString S, int pos, SeqString T, int next[], int* count);
int GetNext(SeqString T, int next[]);
int GetNextVal(SeqString T, int nextval[]);
void PrintArray(SeqString T, int next[], int nextval[], int length);
void main()
{
    SeqString S, T;
    int count1 = 0, count2 = 0, count3 = 0, find;
    int next[40], nextval[40];

    StrAssign(&S, "abaababaddecab");    /*给主串S赋值*/
    StrAssign(&T, "abad");              /*给模式串T赋值*/
    GetNext(T, next);                   /*将next函数值保存在next数组*/
    GetNextVal(T, nextval);             /*将改进后的next函数值保存在nextval数组*/
    printf("模式串T的next和改进后的next值:\n");
    PrintArray(T, next, nextval, StrLength(T)); /*输出模式串T的next值与nextval值*/
    find = B_FIndex(S, 1, T, &count1);          /*传统的模式串匹配*/

    if (find > 0) {
        printf("Brute-Force算法的比较次数为:%2d\n", count1);
    }

    find = KMP_Index(S, 1, T, next, &count2);

    if (find > 0) {
        printf("利用next的KMP算法的比较次数为:%2d\n", count2);
    }

    find = KMP_Index(S, 1, T, nextval, &count3);

    if (find > 0) {
        printf("利用nextval的KMP匹配算法的比较次数为:%2d\n", count3);
    }

    StrAssign(&S, "cbccccbcacbccbacbccbcbcbc"); /*给主串S赋值*/
    StrAssign(&T, "cbccbcbc");                  /*给模式串T赋值*/
    GetNext(T, next);                           /*将next函数值保存在next数组*/
    GetNextVal(T,
               nextval);                     /*将改进后的next函数值保存在nextval数组*/
    printf("模式串T的next和改进后的next值:\n");
    PrintArray(T, next, nextval, StrLength(T)); /*输出模式串T的next值域nextval值*/
    find = B_FIndex(S, 1, T, &count1);          /*传统的模式串匹配*/

    if (find > 0) {
        printf("Brute-Force算法的比较次数为:%2d\n", count1);
    }

    find = KMP_Index(S, 1, T, next, &count2);

    if (find > 0) {
        printf("利用next的KMP算法的比较次数为:%2d\n", count2);
    }

    find = KMP_Index(S, 1, T, nextval, &count3);

    if (find > 0) {
        printf("利用nextval的KMP匹配算法的比较次数为:%2d\n", count3);
    }
}


int B_FIndex(SeqString S, int pos, SeqString T, int* count)
/*在主串S中的第pos个位置开始查找子串T，如果找到返回子串在主串的位置；否则，返回-1*/
{
    int i, j;
    i = pos - 1;
    j = 0;
    *count = 0;                     /*count保存主串与模式串的比较次数*/

    while (i < S.length && j < T.length) {
        if (S.str[i] ==
            T.str[j]) { /*如果串S和串T中对应位置字符相等，则继续比较下一个字符*/
            i++;
            j++;
        } else {                /*如果当前对应位置的字符不相等，则从串S的下一个字符开始，T的第0个字符开始比较*/
            i = i - j + 1;
            j = 0;
        }

        (*count)++;
    }

    if (j >= T.length) {            /*如果在S中找到串T，则返回子串T在主串S的位置*/
        return i - j + 1;
    } else {
        return -1;
    }
}
int KMP_Index(SeqString S, int pos, SeqString T, int next[], int* count)
/*KMP模式匹配算法。利用模式串T的next函数在主串S中的第pos个位置开始查找子串T，如果找到返回子串在主串的位置；否则，返回-1*/
{
    int i, j;
    i = pos - 1;
    j = 0;
    *count = 0;                             /*count保存主串与模式串的比较次数*/

    while (i < S.length && j < T.length) {
        if (j == -1 ||
            S.str[i] == T.str[j]) { /*如果j=-1或当前字符相等，则继续比较后面的字符*/
            i++;
            j++;
        } else {                    /*如果当前字符不相等，则将模式串向右移动*/
            j = next[j];
        }

        (*count)++;
    }

    if (j >= T.length) {                /*匹配成功，返回子串在主串中的位置。否则返回-1*/
        return i - T.length + 1;
    } else {
        return -1;
    }
}
int GetNext(SeqString T, int next[])
/*求模式串T的next函数值并存入数组next*/
{
    int j, k;
    j = 0;
    k = -1;
    next[0] = -1;

    while (j < T.length) {
        if (k == -1 ||
            T.str[j] ==
            T.str[k]) { /*如果k=-1或当前字符相等，则继续比较后面的字符并将函数值存入到next数组*/
            j++;
            k++;
            next[j] = k;
        } else {                    /*如果当前字符不相等，则将模式串向右移动继续比较*/
            k = next[k];
        }
    }
}
int GetNextVal(SeqString T, int nextval[])
/*求模式串T的next函数值的修正值并存入数组next*/
{
    int j, k;
    j = 0;
    k = -1;
    nextval[0] = -1;

    while (j < T.length) {
        if (k == -1 ||
            T.str[j] ==
            T.str[k]) { /*如果k=-1或当前字符相等，则继续比较后面的字符并将函数值存入到nextval数组*/
            j++;
            k++;

            if (T.str[j] !=
                T.str[k]) {     /*如果所求的nextval[j]与已有的nextval[k]不相等，则将k存放在nextval中*/
                nextval[j] = k;
            } else {
                nextval[j] = nextval[k];
            }
        } else {                            /*如果当前字符不相等，则将模式串向右移动继续比较*/
            k = nextval[k];
        }
    }
}
void PrintArray(SeqString T, int next[], int nextval[], int length)
/*模式串T的next值与nextval值输出函数*/
{
    int j;
    printf("j:\t\t");

    for (j = 0; j < length; j++) {
        printf("%3d", j);
    }

    printf("\n");
    printf("模式串:\t\t");

    for (j = 0; j < length; j++) {
        printf("%3c", T.str[j]);
    }

    printf("\n");
    printf("next[j]:\t");

    for (j = 0; j < length; j++) {
        printf("%3d", next[j]);
    }

    printf("\n");
    printf("nextval[j]:\t");

    for (j = 0; j < length; j++) {
        printf("%3d", nextval[j]);
    }

    printf("\n");
}
