#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

typedef struct polyn {
    float coef;      /*存放一元多项式的系数*/
    int expn;        /*存放一元多项式的指数*/
    struct polyn* next;
} PolyNode, *PolyNomial;

PolyNomial CreatePolyn()
/*创建一元多项式*/
{

    PolyNode* p, *q, *s;
    PolyNode* head = NULL;
    int expn2;
    float coef2;
    head = (PolyNomial)malloc(sizeof(PolyNode));

    if (!head) {
        return NULL;
    }

    head->coef = 0;
    head->expn = 0;
    head->next = NULL;

    do {
        printf("输入系数coef");
        scanf("%f", &coef2);
        printf("输入指数exp(输入0 0 结束)");
        scanf("%d", &expn2);

        if ((long)coef2 == 0 && expn2 == 0) {
            break;
        }

        s = (PolyNode*)malloc(sizeof(PolyNode));

        if (!s) {
            return NULL;
        }

        s->expn = expn2;
        s->coef = coef2;
        q = head->next;
        p = head;

        /*创建一个多项式，使多项式按照系数递减排列*/
        while (q && expn2 < q->expn) {
            p = q;
            q = q->next;
        }

        if (q == NULL || expn2 > q->expn) {
            p->next = s;
            s->next = q;
        } else {            /*如果指数相同，合并同类项*/
            q->coef += coef2;
        }
    } while (1);

    return head;
}
PolyNode* Reverse(PolyNomial head)
/*逆置链表*/
{
    PolyNode* q, *r, *p = NULL;
    q = head->next;

    while (q) {
        r = q->next;
        q->next = p;
        p = q;
        q = r;
    }

    head->next = p;
    return head;
}

PolyNode* MultiplyPolyn(PolyNomial A, PolyNomial B)
/*一元多项式的相乘*/
{
    PolyNode* pa, *Pb, *Pc, *u, *head;
    int k, maxExp;
    float coef;
    head = (PolyNomial)malloc(sizeof(PolyNode));

    if (!head) {
        return NULL;
    }

    head->coef = 0.0;
    head->expn = 0;
    head->next = NULL;

    if (A->next != NULL && B->next != NULL) { /*求出两个多项式乘积最大的指数项*/
        maxExp = A->next->expn + B->next->expn;
    } else {
        return head;
    }

    Pc = head;
    B = Reverse(B);                     /*将多项式B逆置*/

    for (k = maxExp; k >= 0; k--) {
        pa = A->next;

        while (pa != NULL && pa->expn > k) {
            pa = pa->next;
        }

        Pb = B->next;

        while (Pb != NULL && pa != NULL && pa->expn + Pb->expn < k) {
            Pb = Pb->next;
        }

        coef = 0.0;

        while (pa != NULL && Pb != NULL) {

            if (pa->expn + Pb->expn == k) {
                coef += pa->coef * Pb->coef;
                pa = pa->next;
                Pb = Pb->next;
            } else if (pa->expn + Pb->expn > k) { /*说明乘积没有等于k的指数项*/
                pa = pa->next;
            } else {
                Pb = Pb->next;
            }
        }

        if (coef != 0.0) {
            u = (PolyNode*)malloc(sizeof(PolyNode));
            u->coef = coef;
            u->expn = k;
            u->next = Pc->next;
            Pc->next = u;
            Pc = u;
        }
    }

    B = Reverse(B);
    return head;
}
void OutPut(PolyNomial head)
/*输出一元多项式*/
{
    PolyNode* p = head->next;

    while (p) {
        printf("%1.1f", p->coef);

        if (p->expn) {
            printf("*x^%d", p->expn);
        }

        if (p->next && p->next->coef > 0) {
            printf("+");
        }

        p = p->next;
    }
}

void main()
{
    PolyNomial A, B, C;
    A = CreatePolyn();
    printf("A(x)=");
    OutPut(A);
    printf("\n");
    B = CreatePolyn();
    printf("B(x)=");
    OutPut(B);
    printf("\n");
    C = MultiplyPolyn(A, B);
    printf("C(x)=A(x)*B(x)=");
    OutPut(C);
    printf("\n");
}
