#include <stdio.h>
#include <stdlib.h>

/*
La funzione main(), come accennato nel sorgente dedicato alla terminazione dei
processi, e' una funzione essenziale, dal punto di vista dello sviluppatore
ad esempio e' possibile accedere alle informazioni passate dall'utente
attraverso i parametri argc ed argv[] della funzione main() stessa; argc e' un
intero ed indica il numero di argomenti, argv[] invece e' un puntatore ad un
vettore di stringhe, ossia ognuno degli elementi che lo costituiscono referenzia
una stringa contenente uno dei parametri passati dalla shell.

Note:
- argv[0] corrisponde sempre, ovviamente, al programma in esecuzione;
- argv[argc] e' sempre un puntatore nullo che indica la fine del vettore;
- argv[argc-1] referenzia l'ultimo parametro;
- argc e argv sono nomi convenzionali, si consiglia di non modificarli.
*/


int main(int argc, char* argv[])
{
    int i;

    printf("Nome del programma: %s\n", argv[0]);
    printf("E' stato invocato con %d elementi\n", argc);

    if (argc > 1) {
        printf("Gli argomenti sono: \n");

        for (i = 0; i < argc; ++i) {
            printf("argv[%i] %s\n", i, argv[i]);
        }
    }

    return (EXIT_SUCCESS);
}
