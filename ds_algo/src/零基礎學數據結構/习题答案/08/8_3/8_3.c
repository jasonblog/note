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
int Count(GList L);
void main()
{
    GList L;
    SeqString S;
    int c;
    StrAssign(&S, "(a,(b),(a,(b,c),d))");   /*将字符串赋值给串S*/
    CreateList(&L, S);                      /*由串创建广义表L*/
    printf("广义表L:\n");
    PrintGList(L);                          /*输出广义表中的元素*/
    printf("\n");
    c = Count(L);
    printf("广义表L中原子的个数：count=%2d\n", c);

}
int Count(GList L)
/*统计原子结点的个数*/
{
    int n1, n2;

    if (L == NULL) {        /*如果广义表为空，则返回0*/
        return 0;
    } else {
        if (L->tag == ATOM) { /*如果是原子，则还需要检查该原子是否有兄弟结点*/
            return 1 + Count(L->tp);
        } else {            /*如果是子表，则需要计算兄弟结点个数与下一层原子结点个数的和*/
            return Count(L->tp) + Count(L->ptr.hp);
        }
    }
}

void PrintGList(GList L)
/*以广义表的形式输出*/
{
    if (L->tag == LIST) {
        printf("(");            /*如果子表存在，先输出左括号 */

        if (L->ptr.hp == NULL) { /*如果子表为空，则输出' '字符 */
            printf(" ");
        } else {                /*递归输出表头*/
            PrintGList(L->ptr.hp);
        }

        printf(")");            /*在子表的最后输出右括号 */
    } else {                    /*如果是原子，则输出结点的值*/
        printf("%c", L->ptr.atom);
    }

    if (L->tp != NULL) {

        printf(", ");           /*输出逗号*/
        PrintGList(L->tp);      /*递归输出表尾*/
    }
}
void CreateList(GList* L, SeqString S)
/*采用扩展线性链表创建广义表*/
{
    SeqString Sub, HeadSub, Empty;
    GList p, q;
    StrAssign(&Empty, "()");

    if (!(*L = (GList)malloc(sizeof(GLNode)))) { /*为广义表生成一个结点*/
        exit(-1);
    }

    if (!StrCompare(S,
                    Empty)) {                /*如果输入的串是空串则创建一个空的广义表*/
        (*L)->tag = LIST;
        (*L)->ptr.hp = (*L)->tp = NULL;
    } else {

        if (StrLength(S) ==
            1) {                    /*广义表是原子，则将原子的值赋值给广义表结点*/
            (*L)->tag = ATOM;
            (*L)->ptr.atom = S.str[0];
            (*L)->tp = NULL;
        } else {                                    /*如果是子表*/
            (*L)->tag = LIST;
            (*L)->tp = NULL;
            SubString(&Sub, S, 2, StrLength(S) - 2); /*将S去除最外层的括号，然后赋值给Sub*/
            DistributeString(&Sub,
                             &HeadSub);       /*将Sub分离出表头和表尾分别赋值给HeadSub和Sub*/
            CreateList(&((*L)->ptr.hp), HeadSub);   /*递归调用生成广义表*/
            p = (*L)->ptr.hp;

            while (!StrEmpty(
                       Sub)) {                /*如果表尾不空，则生成结点p，并将尾指针域指向p*/
                DistributeString(&Sub, &HeadSub);
                CreateList(&(p->tp), HeadSub);
                p = p->tp;
            }

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
