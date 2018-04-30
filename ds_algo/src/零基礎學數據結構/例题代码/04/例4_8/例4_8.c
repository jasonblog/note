#include<stdio.h>
#include<string.h>
typedef char DataType;
#include"SeqStack.h"
#define MaxSize 50
typedef struct { /*操作数栈的类型定义*/
    float data[MaxSize];
    int top;
} OpStack;
void TranslateExpress(char s1[], char s2[]);
float ComputeExpress(char s[]);
void main()
{
    char a[MaxSize], b[MaxSize];
    float f;
    printf("请输入一个算术表达式：\n");
    gets(a);
    printf("中缀表达式为：%s\n", a);
    TranslateExpress(a, b);
    printf("后缀表达式为：%s\n", b);
    f = ComputeExpress(b);
    printf("计算结果：%f\n", f);
}
float ComputeExpress(char a[])
/*计算后缀表达式的值*/
{
    OpStack S;
    int i = 0, value;
    float x1, x2;
    float result;
    S.top = -1;

    while (a[i] != '\0') {
        if (a[i] != ' ' && a[i] >= '0' &&
            a[i] <= '9') { /*如果当前字符是数字字符，则将其转换为数字并存入栈中*/
            value = 0;

            while (a[i] != ' ') {
                value = 10 * value + a[i] - '0';
                i++;
            }

            S.top++;
            S.data[S.top] = value;
        } else {        /*如果当前字符是运算符，则对栈中的数据进行求值，并将结果保存到栈中*/
            switch (a[i]) {

            case '+':
                x1 = S.data[S.top];
                S.top--;
                x2 = S.data[S.top];
                S.top--;
                result = x1 + x2;
                S.top++;
                S.data[S.top] = result;
                break;

            case '-':
                x1 = S.data[S.top];
                S.top--;
                x2 = S.data[S.top];
                S.top--;
                result = x2 - x1;
                S.top++;
                S.data[S.top] = result;
                break;

            case '*':
                x1 = S.data[S.top];
                S.top--;
                x2 = S.data[S.top];
                S.top--;
                result = x1 * x2;
                S.top++;
                S.data[S.top] = result;
                break;

            case '/':
                x1 = S.data[S.top];
                S.top--;
                x2 = S.data[S.top];
                S.top--;
                result = x2 / x1;
                S.top++;
                S.data[S.top] = result;
                break;
            }

            i++;
        }
    }

    if (!S.top != -1) {
        result = S.data[S.top];
        S.top--;

        if (S.top == -1) {
            return result;
        } else {
            printf("表达式错误");
            exit(-1);
        }
    }
}
void TranslateExpress(char str[], char exp[])
/*将中缀表达式转换为后缀表达式*/
{
    SeqStack S;
    char ch;
    DataType e;
    int i = 0, j = 0;
    InitStack(&S);
    ch = str[i];
    i++;

    while (ch != '\0') {
        switch (ch) {
        case'(':    /*左括号入栈*/
            PushStack(&S, ch);
            break;

        case')':    /*如果当前字符是右括号，则将栈中的字符出栈，直到栈中的一个左括号出栈为止*/
            while (GetTop(S, &e) && e != '(') {
                PopStack(&S, &e);
                exp[j] = e;
                j++;
            }

            PopStack(&S, &e); /*左括号出栈*/
            break;

        case'+':
        case'-':
            while (!StackEmpty(S) && GetTop(S, &e) &&
                   e != '(') { /*如果当前字符是+号或-号，则将栈中字符出栈，直到遇到左括号为止*/
                PopStack(&S, &e);
                exp[j] = e;
                j++;
            }

            PushStack(&S, ch); /*将当前字符入栈*/
            break;

        case'*':
        case'/':
            while (!StackEmpty(S) && GetTop(S, &e) && e == '/' ||
                   e == '*') { /*如果当前字符是*号或者是/号，则将栈中字符出栈*/
                PopStack(&S, &e);
                exp[j] = e;
                j++;
            }

            PushStack(&S, ch);  /*当前字符入栈*/
            break;

        case' ':
            break;

        default:                /*处理数字字符*/
            while (ch >= '0' && ch <= '9') {
                exp[j] = ch;
                j++;
                ch = str[i];
                i++;
            }

            i--;
            exp[j] = ' ';
            j++;
        }

        ch = str[i];
        i++;
    }

    while (!StackEmpty(S)) {
        PopStack(&S, &e);
        exp[j] = e;
        j++;
    }

    exp[j] = '\0';
}
