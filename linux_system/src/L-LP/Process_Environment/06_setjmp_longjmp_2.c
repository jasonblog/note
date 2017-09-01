#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

static void f1(int, int, int, int);
static void f2(void);

static jmp_buf jmpbuffer;
static int globval;

/* Il programma mostra il valore delle variabili con ottimizzazione del
compilatore o senza, utilizzando peraltro salti non locali "nonlocal jumps".

Compilare il sorgente normalmente:
$ gcc -std=c11 -Wall -pedantic code.c

E dopo con l'ottimizzazione:
$ gcc -std=c11 -Wall -pedantic -O1 code.c
*/

int main(void)
{
    int autoval;
    register int regival;
    volatile int volaval;
    static int statval;

    globval = 1;
    autoval = 2;
    regival = 3;
    volaval = 4;
    statval = 5;

    if (setjmp(jmpbuffer) != 0) {
        printf("after longjmp:\n");
        printf("globval = %d, autoval = %d, regival = %d, "
               "volaval = %d, statval = %d\n",
               globval, autoval, regival, volaval, statval);

        exit(EXIT_SUCCESS);
    }

    /* cambia le variabili dopo setjmp(), ma prima della longjmp() */
    globval = 95;
    autoval = 96;
    regival = 97;
    volaval = 98;
    statval = 99;

    /* Non ritorna mai */
    f1(autoval, regival, volaval, statval);

    return (EXIT_SUCCESS);
}

static void f1(int i, int j, int k, int l)
{
    printf("in f1():\n");
    printf("globval = %d, autoval = %d, regival = %d, "
           "volaval = %d, statval = %d\n", globval, i, j, k, l);

    f2();
}

static void f2(void)
{
    longjmp(jmpbuffer, 1);
}
