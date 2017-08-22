#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    /* Indirizzo da convertire, nella forma decimale puntata */
    char* ind_dp = "207.97.227.239";

    /* I sistemi operativi che utilizzano lo standard POSIX conservano gli
    indirizzi IP in una variabile di tipo in_addr_t, ossia un intero senza
    segno a 32 bit */
    in_addr_t ind;

    /* La funzione intet_addr() converte un indirizzo internet IPv4 da notazione
    puntata in formato di rete binario. INADDR_NONE e' una costante che indica
    indirizzo errato. */
    if ((ind = inet_addr(ind_dp)) == INADDR_NONE) {
        fprintf(stderr, "Err. inet_addr() - Invalid input: INADDR_NONE\n");
        exit(EXIT_FAILURE);
    }

    /* Da notare tuttavia che il valore di INADDR_NONE e' solitamente -1, e
    poiche' -1 e' un indirizzo valido, ossia 255.255.255.255 l'uso di questa
    funzione e' altamente sconsigliato in favore di inet_aton() e inet_pton().
    */

    /* La funzione inet_aton converte, come inet_addr(), un indirizzo internet
    IPv4 da notazione puntata in formato di rete binario. */
    struct in_addr inp;

    if (inet_aton(ind_dp, &inp) == 0) {
        fprintf(stderr, "Err. inet_aton() %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Essendo in_addr_t un unsigned long int, si fornisce alla printf %u */
    printf("Indirizzo IP formato di rete 32bit: %u\n", ind);

    /* La funzione inet_ntoa() converte un indirizzo internet (formato di rete
    binario) in una stringa nella notazione decimale puntata IPv4 */
    printf("Indirizzo IP formato di rete 32bit: %s\n", inet_ntoa(inp));

    return (EXIT_SUCCESS);
}
