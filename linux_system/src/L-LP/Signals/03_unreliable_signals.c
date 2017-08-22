#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
I segnali possono essere definiti da due tipologie di comportamento, dette
semantiche:
- i segnali affidabili o semantica affidabile "reliable signals";
- i segnali inaffidabili o semantiche inaffidabile "unreliable signals".

Le prime versioni di UNIX fornivano segnali inaffidabili; quando un programma
intercettava un segnale, l'azione registrata per il segnale rimandava a quella
predefinita del sistema. Il "signal handler" procedeva immediatamente a una
nuova registrazione delle azioni richieste, ma in tal modo si esponeva al
rischio che a causa di problemi di sincronizzazione venisse eseguita l'azione
predefinita.

Ovvero: La routine di gestione del segnale soecificata dall'utente non resta
attiva una volta eseguita, per cui dev'essere cura dell'utente ripetere
l'installazione all'interno del segnale nei casi in cui si vuole che resti
attivo.

Per questo motivo l'implementazione dei segnali secondo questa semantica era
considerata inaffidabile; sia la ricezione sia la reinstallazione del relativo
signal handler non sono operazioni atomiche, per cui sono sempre possibili delle
"race condition", inoltre c'e' il rischio che i segnali possano essere perduti.

Un ulteriore problema consiste nel fatto che che non vi e' un modo di bloccare
i segnali quando s'intende non farli arrivare a destinazione; i segnali tuttavia
possono essere ignorati ma non vi e' modo di istruire il sistema a non fare
nulla all'occorrere di un segnale.

Pertanto i problemi noti dei dei segnali inaffidibili sono:
- perdita del segnale o terminazione del processo. La funzione non resta attiva
  dopo l'esecuzione, e' necessario reinstallare il segnale;
- impossibilita' di bloccare un segnale.

Interruzione della computazione
-------------------------------
Alla ricezione di un segnale, la computazione in un processo viene interrotta;
in che modo riprende, una volta completata la fase di gestione del segnale ?

- Se la computazione interrotta era l'esecuzione di una generica istruzione in
  user-mode, essa riprende da dove era stata interrotta, ad esempio una
  istruzione macchina puntata dal program counter prima della gestione del
  segnale;

- Se la computazione interrotta era una system call su cui il processo era
  bloccato, la system call viene abortita, fallisce, e la variabile globale
  errno viene settata con il codice di errore EINTR. La system call abortita non
  viene richiamata automaticamente.

- Le System call che non bloccano il processo sono eseguite in modo atomico e
  non vengono mai interrotte da alcun segnale.
*/

void signal_handler(int signum);

/* Quando un segnale e' catturato, il signal handler setta il flag
segnale_arrivato a un valore diverso da zero */
int segnale_arrivato = 0;

int main(int argc, char* argv[])
{

    signal(SIGUSR1, signal_handler);

    /* Esiste una finestra temporale all'interno della quale la notifica di un
    secondo segnale può far terminare il processo */

    while (!segnale_arrivato)
        /* Questa e' la finestra temporale che potrebbe creare problemi.
        Se dovesse occorrere un nuovo segnale subito dopo la verifica di
        segnale_arrivato ma prima della funzione pause(), il processo potrebbe
        andare in sleep per sempre e il segnale andrebbe perso. */
    {
        pause();    /* Il processo attendera' un ulteriore segnale */
    }

    return (EXIT_SUCCESS);
}

void signal_handler(int signum)
{
    /* Reinstallazione del segnale */
    signal(SIGUSR1, signal_handler);

    segnale_arrivato = 1;
}
