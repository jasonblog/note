#include <stdio.h>
#include <stdlib.h>

/*
Ogni programma ha a dispozione, oltre ai parametri passati alla funzione main(),
una "environment list", ovvero un puntatore ad un vettore di stringhe,
all'interno del quale ciascuna stringa (null-terminated) indica una variabile
d'ambiente, nella forma "nome = valore"; ogni variabile d'ambiente contiene
specifiche informazioni sull'ambiente in cui il programma sta girando.

Le variabili d'ambiente sono settate, generalmente, al login, tuttavia
l'implementazione dipende essclusivamente dal sistema in uso; POSIX si occupa
della definizione delle variabili d'ambiente e non ISO C.

E' possibile accedere alla lista delle variabili d'ambiente "environment list"
mediante la variabile (globale) esterna 'environ', oppure utilizzando un terzo
argomento della funzione main(), envp[], altamente sconsigliato sia da ISO C
sia da POSIX.

Vi sono diversi comandi che consentono di ottenere la lista delle variabili
d'ambiente come 'env' o 'printenv'.
*/

extern char** environ;

int main(void)
{
    char** var;

    /* stampa tutte le variabili di ambiente definite */
    for (var = environ; *var != NULL; ++var) {
        printf("%s\n", *var);
    }

    return (EXIT_SUCCESS);
}

