#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

/*
Salti non locali
----------------
Di norma una funzione ritorna al punto nel quale e' stata invocata, con la
funzione setjmp() invece e' possibile far saltare una funzione direttamente
in un'altra funzione e senza che si effettui un return.

L'header <setjmp.h> mette a disposizione due funzioni, l'una in stretta
correlazione con l'altra, esse sono setjmp() e longjmp(), con le seguenti
caratteristiche:

HEADER    : <setjmp.h>
PROTOTYPE : int setjmp(jmp_buf env);
SEMANTICS : La funzione setjmp() salva lo stack, contesto/ambiente corrente,
            nella variabile 'env', per essere utilizzata successivamente da
            longjmp().
RETURNS   : 0 se chiamata direttamente, non zero se ritorna da longjmp()
--------------------------------------------------------------------------------
HEADER    : <setjmp.h>
PROTOTYPE : int longjmp(jmp_buf env, int val);
SEMANTICS : La funzione longjmp() restituisce l'ambiente salvato dall'ultima
            chiamata a setjmp() mediante la variabile 'env'; dopo aver
            completato l'esecuzione restituisce il controllo a setjmp() che
            restituira' 'val'.
RETURNS   : Non ritorna
--------------------------------------------------------------------------------

La funzione setjmp() fissa una posizione all'interno del programma, salvando
l'ambiente corrente nella variabile 'env' di tipo jmp_buf e ritornando 0, la
funzione longjmp() invece puo' essere adoperata per tornare nella posizione
definita da setjmp(); l'uso delle funzioni su citate tuttavia riguarda
soprattutto la gestione degli errori e degli interrupts.

Dopo aver ripristinato l'ambiente con la variabile 'env' di tipo jmp_buf, la
funzione longjmp() ritornera' dalla chiamata alla setjmp(), questa volta pero'
il valore restituito sara' 'val', per cui se 'val' e' uguale a zero la setjmp()
restituira' uno.

In breve:
- setjmp() ritorna 0 alla prima invocazione;
- successivamente longjmp() trasferisce nuovamente il controllo alla chiamata
  originaria setjmp() che restituira' un valore diverso da zero.
*/

void func_one(void);
void func_two(void);

/* la variabile che conterra' l'ambiente corrente */
jmp_buf env;

int main(int argc, char* argv[])
{
    int ret_sj;

    if ((ret_sj = setjmp(env)) == 0) {
        printf("setjmp() returned: %d\n", ret_sj);
    } else {
        /* Dopo l'invocazione della funzione longjmp() l'esecuzione riprende
                  da questo punto */
        printf("Program terminates: longjmp() called.\n"
               "setjmp() returns: %d\n", ret_sj);
        return (EXIT_SUCCESS);
    }

    func_one();
    printf("program terminates normally\n");

    return (EXIT_SUCCESS);
}

void func_one(void)
{
    printf("func_one() begins\n");
    func_two();

    /* Le istruzioni da questo punto in poi non saranno eseguite */
    printf("func_one() returns\n");
}

void func_two(void)
{
    printf("func_two() begins\n");

    /* Si ritorna all'istruzione successiva alla chiamaa setjmp(), da notare
    che impostando val a 0, si avra' un valore di ritorno comunque diverso da
    0; in questo caso tuttavia si e' optato per un valore facilmente verificale
    al ritorno della setjmp(). */
    longjmp(env, 33);

    /* Le istruzioni da questo punto in poi non saranno eseguite */
    printf("func_two() returns\n");
}
