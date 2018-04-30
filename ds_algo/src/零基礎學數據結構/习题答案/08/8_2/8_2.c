/*包含头文件和广义表、串的基本操作的实现文件*/
#include <stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>
typedef char AtomType;
#include"GList.h"
#include"SeqString.h"
/*函数声明*/
void CreateList(GList* L, SeqString S);
void DistributeString(SeqString* Str, SeqString* HeadStr);
void PrintGList(GList L);
void main()
{
    GList L;
    SeqString S;
    int length;
    StrAssign(&S, "(a,(b),(a,(b,c),d))");   /*将字符串赋值给串S*/
    CreateList(&L, S);                      /*由串创建广义表L*/
    printf("广义表L:\n");
    PrintGList(L);                          /*输出广义表中的元素*/
    printf("\n");
    length = GListLength1(L);
    printf("广义表L的长度(非递归)length=%2d\n", length);
    length = GListLength2(L);
    printf("广义表L的长度(递归)length=%2d\n", length);
}
int GListLength1(GList L)
/*求广义表的长度操作，非递归实现*/
{
    int length = 0;

    while (L) {                         /*如果广义表非空，则将p指向表尾指针，统计表的长度*/
        L = L->ptr.tp;
        length++;
    }

    return length;
}
int GListLength2(GList L)
/*求广义表的长度操作，递归实现*/
{
    if (L == NULL) {
        return 0;
    } else {
        return GListLength2(L->ptr.tp) + 1;
    }
}

void PrintGList(GList L)
/*以广义表形式输出。采用头尾链表表示*/
{
    if (!L) {               /*如果广义表或子表为空，则直接输出一对括号(左括号和右括号)*/
        printf("()");
        return;
    } else if (L->tag == ATOM) { /*如果是原子，则直接输出原子元素*/
        printf("%c", L->atom);
        return;
    } else {                /*如果是子表，先输出一个左括号*/
        printf("(");
    }

    for (; L; L = L->ptr.tp) {
        PrintGList(L->ptr.hp);  /*递归调用输出子表*/

        if (L->ptr.tp) {        /*如果表尾还有元素，则输出一个逗号*/
            printf(",");
        }

    }

    printf(")");                /*如果子表结束，最后输出一个右括号*/
}
void CreateList(GList* L, SeqString S)
/*采用头尾链表创建广义表*/
{
    SeqString Sub, HeadSub, Empty;
    GList p, q;
    StrAssign(&Empty, "()");

    if (!StrCompare(S, Empty)) {        /*如果输入的串是空串则创建一个空的广义表*/
        *L = NULL;
    } else {
        if (!(*L = (GList)malloc(sizeof(GLNode)))) { /*为广义表生成一个结点*/
            exit(-1);
        }

        if (StrLength(S) ==
            1) {                    /*广义表是原子，则将原子的值赋值给广义表结点*/
            (*L)->tag = ATOM;
            (*L)->atom = S.str[0];
        } else {                                    /*如果是子表*/
            (*L)->tag = LIST;
            p = *L;
            SubString(&Sub, S, 2, StrLength(S) - 2); /*将S去除最外层的括号，然后赋值给Sub*/

            do {
                DistributeString(&Sub,
                                 &HeadSub);   /*将Sub分离出表头和表尾分别赋值给HeadSub和Sub*/
                CreateList(&(p->ptr.hp), HeadSub);  /*递归调用生成广义表*/
                q = p;

                if (!StrEmpty(
                        Sub)) {               /*如果表尾不空，则生成结点p，并将尾指针域指向p*/
                    if (!(p = (GLNode*)malloc(sizeof(GLNode)))) {
                        exit(-1);
                    }

                    p->tag = LIST;
                    q->ptr.tp = p;
                }
            } while (!StrEmpty(Sub));

            q->ptr.tp = NULL;
        }
    }
}
void DistributeString(SeqString* Str, SeqString* HeadStr)
/*将串Str分离成两个部分，HeadStr为第一个逗号之前的子串，Str为逗号后的子串*/
{
    int len, i, k;
    SeqString Ch, Ch1, Ch2, Ch3;
    len = StrLength(*Str);              /*len为Str的长度*/
    StrAssign(&Ch1, ",");               /*将字符','、'('和')'分别赋给Ch1,Ch2和Ch3*/
    StrAssign(&Ch2, "(");
    StrAssign(&Ch3, ")");
    SubString(&Ch, *Str, 1, 1);         /*Ch保存Str的第一个字符*/

    for (i = 1, k = 0; i <= len && StrCompare(Ch, Ch1) ||
         k != 0; i++) { /*搜索Str最外层的第一个括号*/
        SubString(&Ch, *Str, i, 1);     /*取出Str的第一个字符*/

        if (!StrCompare(Ch, Ch2)) {     /*如果第一个字符是'('，则令k加1*/
            k++;
        } else if (!StrCompare(Ch, Ch3)) { /*如果当前字符是')'，则令k减去1*/
            k--;
        }
    }

    if (i <= len) {                     /*串Str中存在','，它是第i-1个字符*/
        SubString(HeadStr, *Str, 1, i - 2); /*HeadStr保存串Str','前的字符*/
        SubString(Str, *Str, i, len - i + 1); /*Str保存串Str','后的字符*/
    } else {                            /*串Str中不存在','*/
        StrCopy(HeadStr, *Str);         /*将串Str的内容复制到串HeadStr*/
        StrClear(Str);                  /*清空串Str*/
    }
}
