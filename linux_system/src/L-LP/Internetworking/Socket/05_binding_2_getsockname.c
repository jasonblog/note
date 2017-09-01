#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 0
#define SEC  30

/* Per capire a quale indirizzo di trasporto e' associato un socket, e'
possibile utilizzare la funzione getsockname():

HEADER    : <sys/socket.h>
PROTOTYPE : int getsockname(int sockfd, struct sockaddr *addr, socklen_t *len);
SEMANTICS : La funzione getsockname() restituisce l'indirizzo corrente al quale
            il socket 'sockfd' e' associato, nel buffer puntato da 'addr'. 'len'
            sara' inizializzato con la dimensione in byte dell'indirizzo del
            socket.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

/* Il programma associa un socket ad una porta e ne stampa il numero  */
int main(int argc, char* argv[])
{
    struct sockaddr_in address;     /* Ipv4 internet address */
    struct sockaddr_in local_addr;
    struct protoent* pr;
    int sock;
    unsigned int len = sizeof(local_addr);

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <protocollo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Si definisce il protocollo da adoperare */
    if ((pr = getprotobyname(argv[1])) == NULL) {
        fprintf(stderr, "Err. getprotobyname()\n");
        exit(EXIT_FAILURE);
    }

    /* Si crea il socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, pr->p_proto)) < 0) {
        fprintf(stderr, "Err. socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si stabilisce l'indirizzo di trasporto con cui fare l'associazione;
    se non si intende assegnare una specifica interfaccia si puo' utilizzare
    INADDR_ANY.  */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    /* Si pone la porta a 0 per far si che il binding avvenga alla prima porta
    disponibile */
    address.sin_port = htons(PORT);

    if (bind(sock, (void*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Err. bind() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (getsockname(sock, (void*)&local_addr, &len) < 0) {
        fprintf(stderr, "Err. getsockname() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si stampa l'indirizzo, non prima di averlo convertito mediante ntohs() */
    printf("Porta allocata: %d\n", ntohs(local_addr.sin_port));

    sleep(SEC);

    return (EXIT_SUCCESS);
}
