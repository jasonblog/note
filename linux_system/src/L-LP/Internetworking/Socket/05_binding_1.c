#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 7000
#define SEC 30

/* Quando un socket e' creato mediante la funzione socket() non ancora gli si
assegna un indirizzo, tale compito e' cura della funzione bind(), l'associazione
tra socket ed indirizzo di trasporto prende il nome di binding.

HEADER    : <sys/socket.h> <sys/types.h>
PROTOTYPE : int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
SEMANTICS : La funzione bind() assegna l'indirizzo 'addr' al socket 'sockfd',
            'len' definisce la lunghezza dell'indirizzo puntato da 'addr'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

/* Il programma associa un socket alla porta PORT  */
int main(int argc, char* argv[])
{
    struct sockaddr_in address;     /* Ipv4 internet address */
    struct protoent* pr;
    int sock;

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
    address.sin_port = htons(PORT);

    if (bind(sock, (void*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Err. bind() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sleep(SEC);

    return (EXIT_SUCCESS);
}

/* Utilizzando netstat si potra' verificare l'allocazione del socket */
