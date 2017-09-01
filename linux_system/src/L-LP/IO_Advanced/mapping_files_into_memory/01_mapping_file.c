#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* Il kernel linux consente ad ogni processo di mappare un file su disco
direttamente in memoria, creando in tal modo una relazione tra i byte del file
su disco e i byte della memoria.

Il vantaggio principale di questa tecnica e' che i file mappati in memoria sono
piu' veloci di quelli su disco; inoltre i dati sono condivisi, per cui processi
multipli possono accedere alle medesime informazioni.

La funzione mmap() consente il mapping di un file:

HEADER   : <sys/mman.h>
PROTOTYPE: void *mmap(void *addr, size_t length, int prot, int flags,
                      int fd, off_t offset);

Il file aperto associato al file descriptor 'fd' e' mappato a partire dall'
indirizzo 'addr', se 'addr' dovesse valere NULL sara' il kernel stesso a
definire l'indirizzo dal quale iniziare la mappatura. La dimensione del file da
mappare e' definita da 'length'. La protezione del range di memoria e'
stabilito da 'prot', i cui valori riguardano lettura 'PROT_READ', scrittura
'PROT_WRITE', esecuzione 'PROT_EXEC' e accesso non consentito 'PROT_NONE'. Il
tipo di memoria e' impostato mediante la variabile intera 'flags'.

Il file viene suddiviso in pagine multiple - tipicamente da 4096 Kb -
copiandole nelle pagine della memoria virtuale, rendendole pertanto disponibili
nello spazio degli indirizzi del processo.

La system call mmap() restituisce un puntatore alla regione di memoria
associata in caso di successo, -1 altrimenti. */

int main(int argc, char* argv[])
{
    char* text = "Take a Walk on the Wild Side (Lou Reed)";
    void* fmem;
    int fin;
    size_t len = strlen(text);

    if ((fin = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Si usa lseek() per assicurarsi che il file sia sufficientemente capiente
    if (lseek(fin, len, SEEK_SET) == -1) {
        fprintf(stderr, "Err.(%d) lseek() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (write(fin, "", 1) == -1) {
        fprintf(stderr, "Err.(%d) write() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Mappa il file 'fin' in memoria
    fmem = mmap(0, len, PROT_WRITE, MAP_SHARED, fin, 0);

    if (fmem == MAP_FAILED) {
        fprintf(stderr, "Err.(%d) mmap() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(fin);

    // Scrive il messaggio direttamente in memoria
    sprintf((char*)fmem, "%s", text);

    if (munmap(fmem, len) == -1) {
        fprintf(stderr, "Err.(%d) munmap() - %s\n", errno, strerror(errno));
        exit(EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}
