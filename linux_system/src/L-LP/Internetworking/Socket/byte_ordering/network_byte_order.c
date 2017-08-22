#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Differenti architetture possono utilizzare differenti metodi di byte-ordering
, come e' noto i processori che supportano il big-endian conservano il byte piu'
significativo nell'indirizzo piu' basso, mentre i processori con supporto
little-endian piazzano nell'indirizzo piu' basso il byte meno significativo.

Questo potrebbe generare, come e' ovvio, parecchia confusione quando e'
necessaria una comunicazione tra macchine con architetture differenti; per
evitare questo genere di problemi l'interfaccia dei socket specifica uno
standard byte-ordering chiamato network byte order, implementato peraltro come
big-endian.

Nota: Secondo logica tutte le comunicazioni di rete dovrebbero essere big-endian

I socket forniscono una serie di macro utili alla conversione da host byte-order
a network byte-order e viceversa, esse sono:
(h = host, n = network, l = long, s = short)

HEADER    : <arpa/inet.h>
PROTOTYPE : uint32_t htonl(uint32_t hostint32);
SEMANTICS : La funzione htonl() converte 'hostint32' in network byte-order
RETURNS   : Un intero da 32bit
--------------------------------------------------------------------------------
HEADER    : <arpa/inet.h>
PROTOTYPE : uint16_t htons(uint16_t hostint16);
SEMANTICS : La funzione htonl() converte 'hostint16' in network byte-order
RETURNS   : Un intero da 16bit
--------------------------------------------------------------------------------
HEADER    : <arpa/inet.h>
PROTOTYPE : uint32_t ntohl(uint32_t netint32);
SEMANTICS : La funzione htonl() converte 'netint32' in host byte-order
RETURNS   : Un intero da 32bit
--------------------------------------------------------------------------------
HEADER    : <arpa/inet.h>
PROTOTYPE : uint16_t ntohs(uint16_t netint16);
SEMANTICS : La funzione htons() converte 'netint16' in host byte-order
RETURNS   : Un intero da 16bit
--------------------------------------------------------------------------------
*/

int main(void)
{
    int i;
    long x = 0x78A27D0;
    unsigned char* ptr = (char*)&x;

    printf("Valori in host byte order\n");
    printf(" x in Esadecimale: %x\n", x);
    printf("           x byte: ");

    for (i = 0; i < sizeof(long); i++) {
        printf("%x\t", ptr[i]);
    }

    printf("\n\n");

    printf("Valori in network byte order\n");
    x = htonl(x);
    printf("dopo la conversione con htonl()\n");
    printf(" x in Esadecimale: %x\n", x);
    printf("           x byte: ");

    for (i = 0; i < sizeof(long); i++) {
        printf("%x\t", ptr[i]);
    }

    /* Se il processore che si sta utilizzando e' little-endian, i risultati
    saranno differenti rispetto alla prima iterazione, se invece e' big-endian,
    la conversione htonl() non produrra' effetti. */

    return (EXIT_SUCCESS);
}
