#include<stdio.h>
#define MaxLength 60
typedef struct { /*串的定义*/
    char str[MaxLength];
    int length;
} SeqString;


void StrAssign(SeqString* S, char* cstr)
/*串的赋值操作*/
{
    int i = 0;

    for (i = 0; cstr[i] != '\0'; i++) { /*将常量cstr中的字符赋值给串S*/
        S->str[i] = cstr[i];
    }

    S->length = i;
}
int StrEmpty(SeqString S)
/*判断串是否为空，串为空返回1，否则返回0*/
{
    if (S.length == 0) {    /*判断串的长度是否等于0*/
        return 1;    /*当串为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}

int StrIndex(SeqString S, int pos, SeqString T)
/*在主串S中的第pos个位置开始查找子串T，如果找到返回子串在主串的位置；否则，返回-1*/
{
    int i, j;

    if (StrEmpty(T)) {              /*如果串T为空，则返回0*/
        return 0;
    }

    i = pos;
    j = 0;

    while (i < S.length && j < T.length) {
        if (S.str[i] ==
            T.str[j]) { /*如果串S和串T中对应位置字符相等，则继续比较下一个字符*/
            i++;
            j++;
        } else {                /*如果当前对应位置的字符不相等，则从串S的下一个字符开始，T的第0个字符开始比较*/
            i = i - j + 1;
            j = 0;
        }
    }

    if (j >= T.length) {            /*如果在S中找到串T，则返回子串T在主串S的位置*/
        return i - j + 1;
    } else {
        return -1;
    }
}

int DelString(SeqString* S, int pos, int len)
/*在串S中删除pos开始的len个字符*/
{
    int i;

    if (pos < 0 || len < 0 ||
        pos + len - 1 > S->length) { /*如果参数不合法，则返回0*/
        printf("删除位置不正确，参数len不合法");
        return 0;
    } else {
        for (i = pos + len; i <= S->length - 1;
             i++) { /*将串S的第pos个位置以后的len个字符覆盖掉*/
            S->str[i - len] = S->str[i];
        }

        S->length = S->length - len;            /*修改串S的长度*/
        return 1;
    }
}
void StrPrint(SeqString S)
/*串S的输出*/
{
    int i;

    for (i = 0; i < S.length; i++) {
        printf("%c", S.str[i]);
    }

    printf("\n");
}
void main()
{
    SeqString S1, S2, Sub;
    char ch1[] = "abcaabcbcacbbc";
    char ch2[] = "caabc";
    int n;
    StrAssign(&S1, ch1);            /*将字符串赋给串S1*/
    printf("第一个串s1:");
    StrPrint(S1);
    StrAssign(&S2, ch2);            /*将字符串赋给串S2*/
    printf("第二个串s2:");
    StrPrint(S2);
    n = StrIndex(S1, 1, S2);    /*在串S1中查找串S2的位置*/
    printf("在串s2在串s1的位置为:%2d\n", n);
    printf("在串s1中删除s2后，串s1为:");

    if (n >= 0) {
        DelString(&S1, 1, S2.length);    /*在串S1中删除串S2*/
    }

    StrPrint(S1);

}

