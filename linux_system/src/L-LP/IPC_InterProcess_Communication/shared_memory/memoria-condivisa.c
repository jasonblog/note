#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
    /* Identificatore del segmento condiviso */
    int segment_id;

    /* Puntatore al segmento condiviso */
    char* shared_memory;

    /* Si esprime in byte la dimensione del segmento condiviso */
    const int size = 1024;

    /*
     * Come prima operazione si deve allocare un segmento condiviso di memoria
     * mediante la funzione:
     * shmget() - Shared Memory Get | Acquisisci Memoria Condivisa.
     *
     * - 1° Parametro 'IPC_PRIVATE'
     * Serve per specificare o la chiave o l'ID del segmento condiviso; nel caso
     * specifico si alloca un nuovo segmento di memoria condivisa.
     *
     * - 2° Parametro 'size'
     * Riguarda la dimensione in byte del segmento condiviso, precedentemente
     * dichiarato come costante intero ed inizializzato a 4096 byte.
     *
     * - 3° Parametro S_IRUSR | S_IWUSR
     * Specifica il modo di accesso, in questo caso e' in lettura e scrittura.
     *
     * Qualora la chiamata a shmegt() vada a buon fine, il valore di ritorno
     * sara' un intero, identificativo del segmento allocato.
     */

    segment_id = shmget(IPC_PRIVATE, size, S_IRUSR | S_IWUSR);

    /*
     * Tutti i processi che volessero accedere al segmento di memoria condivisa,
     * dovranno annetterlo nel loro spazio di indirizzamento; questa operazione
     * la sei esegue mediante la funzione:
     * shmat() - Shared Memory Attack | Annetti Memoria Condivisa.
     *
     * 1° Parametro 'segment_id'
     * ID intero del segmento da annettere
     *
     * 2° Parametro 'NULL'
     * Puntatore che indica dove annettere il segmento; con NULL si lascia tale
     * incombenza al Sistema Operativo.
     *
     * 3° Parametro '0'
     * Specifica il modo di accesso, 0 sola lettura, >0 lettura e scrittura.
     *
     * Qualora la chiamata a shmat() vada a buon fine, restituira' un puntatore
     * alla prima locazione di memoria dove e' stato annesso il segmento
     * condiviso.
     */
    shared_memory = (char*) shmat(segment_id, NULL, 0);

    /*
     * Avendo questo puntatore, il processo puo' accedere alla memoria annessa e,
     * considerato che si tratta di un puntatore char * - stringa di caratteri -
     * si puo' utilizzare tale memoria per scriverci una stringa.
     */

    sprintf(shared_memory, "Interprocesso Communication - IPC");

    /*
     * Si puo' inoltre stampare la stringa contenuta nella memoria condivisa
     */

    printf("*%s\n", shared_memory);

    /*
     * Quando un processo non ha piu' bisogno di condividere un segmento di
     * memoria, lo si elimina dal proprio spazio degli indirizzi, utilizzando la
     * funzione:
     * shmdt() - Shared Memory Delete | Elimina Memoria Condivisa.
     *
     * In questo caso 'shared_memory' rappresenta il segmento di memoria da
     * disconnettere.
     */

    shmdt(shared_memory);

    /*
     * Infine, un segmento di memoria puo' essere eliminato dall'intero sistema
     * utilizzando la funzione:
     * shmctl() -
     *
     * 1° Parametro 'segment_id'
     * L'identificatore del segmento di memoria
     *
     * 2° Parametro 'IPC_RMID'
     * Si possono passare diversi comandi di controllo, IPC_RMID elimina il
     * segmentodi memoria condivisa specificato da shmid().
     *
     * 3° Parametro 'NULL'
     * Puntatore alla struttura shmid_ds che si occupa di gestire le informazioni
     * di uno specifico segmento di memoria.
     */

    shmctl(segment_id, IPC_RMID, NULL);

    return (EXIT_SUCCESS);
}
