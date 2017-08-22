#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
I segnali affidabili "realiable signals" non hanno tutti i problemi che si
possono verificare con i segnali inaffidabili "unreliable signals"[1].

Nota: E' la semantica adottata dagli unix moderni e Linux.

Nella gestione affidabile dei segnali il "signal handler" una volta installato
resta attivo per tutta la durata del processo, i segnali sono generati dal
kernel per un processo all'occorrenza di un evento, che causa anche il settaggio
del relativo flag nella "process table".

Terminologia
------------
- generated, quando occorre un evento che genera un segnale si dice che un
             segnale e' stato generato per un processo. E' questo il momento
         in cui viene settato il flag nella process table;

- delivered, quando viene eseguita l'azione prevista per un segnale si dice che
             il segnale e' stato consegnato al processo;

- pending  , il tempo che trascorre tra la generazione del segnale e la relativa
             consegna al processo si dice che il segnale e' pendente.

Bloccare la consegna
--------------------
Un processo puo' bloccare la consegna dei segnali; se l'azione prevista per il
segnale e' il comportamento di default o la cattura del segnale, il segnale
resta pendente fintanto che il processo non lo sblocca o cambia l'azione facendo
in modo che possa ignorarlo.

Nota: Il kernel decide cosa fare del segnale bloccato, alla consegna del segnale
      stesso e non quando il segnale e' stato generato, consentendo in tal modo
      il cambiamento dell'azione del segnale prima che venga consegnato.

La funzione sigpending() consente di determinare quali segnali sono bloccati e
quali sono pendenti.

Signal mask
-----------
Il blocco dei segnali viene eseguito impostando opportunamente i bit della
maschera del processo "signal mask", essa rappresenta l'insieme dei segnali che
il processo sta bloccando.

Un processo puo' verificare ed eventualmente cambiare i valori della signal mask
invocando la funzione sigprocmask(); sigset_t rappresenta la struttura dati
specifica per tale circostanza.
*/

int main(int argc, char* argv[])
{
    return (EXIT_SUCCESS);
}
/*
[1] I segnali inaffidabili sono stati trattati nel sorgente:
../Signals/03_unreliable_signals.c
*/
