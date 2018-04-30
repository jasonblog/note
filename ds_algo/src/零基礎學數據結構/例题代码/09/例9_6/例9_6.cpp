#include "stdio.h"
#include "stdlib.h"
typedef struct node {
    char data;
    struct node* lchild;
    struct node* rchild;
} NODE, *NODEP;

void MidOrder(NODEP p);
NODEP rebacktree(NODEP t, char a[], char b[], int al, int ah, int bl, int bh);
NODEP rebacktree1(NODEP t, char a[], char b[], int al, int ah, int bl, int bh);
void main()
{
    /*char a[]="ABDEGHCFIJ";  */
    char a[] = "DGHEBJIFCA";
    char b[] = "DBGEHACIJF";
    NODEP r = NULL;
    r = rebacktree1(r, a, b, 0, 9, 0, 9);
    MidOrder(r);
    printf("\n");
}

void MidOrder(NODEP p)
{
    if (p) {
        MidOrder(p->lchild);
        printf("%c ", p->data);
        MidOrder(p->rchild);
    }
}

NODEP rebacktree(NODEP t, char a[], char b[], int al, int ah, int bl, int bh)
{
    NODEP r;
    int j;

    if (al <= ah) {
        r = (NODEP)malloc(sizeof(NODE));
        r->data = a[al];
        r->lchild = NULL;
        r->rchild = NULL;
        t = r;

        for (j = bl; j <= bh; j++) {
            if (a[al] == b[j]) {
                break;
            }
        }

        t->lchild = rebacktree(t->lchild, a, b, al + 1, al + j - bl, bl, j - 1);
        t->rchild = rebacktree(t->rchild, a, b, al + j - bl + 1, ah, j + 1, bh);
        return t;
    }

    return NULL;
}
NODEP rebacktree1(NODEP t, char a[], char b[], int al, int ah, int bl, int bh)
{
    NODEP r;
    int j;

    if (al <= ah) {
        r = (NODEP)malloc(sizeof(NODE));
        r->data = a[ah];
        r->lchild = NULL;
        r->rchild = NULL;
        t = r;

        for (j = bl; j <= bh; j++) {
            if (a[ah] == b[j]) {
                break;
            }
        }

        t->lchild = rebacktree1(t->lchild, a, b, al, al + j - bl - 1, bl, j - 1);
        t->rchild = rebacktree1(t->rchild, a, b, al + j - bl, ah - 1, j + 1, bh);
        return t;
    }

    return NULL;
}

