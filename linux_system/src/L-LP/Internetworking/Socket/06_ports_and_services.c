#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>

/* Per risalire al numero di porta avendo il nome di un servizio, e' possibile
utilizzare la funzione getservbyname(), che restituira' un puntatore ad una
sruttura servent, contenente oltre al numero di porta diverse voci sul servizio:

struct servent {
    char    *s_name;        - nome del servizio
    char    **s_aliases     - lista degli alias
    int     s_port;         - il numero di porta
    char    *s_proto;       - il protocollo
};

HEADER    : <netdb.h>
PROTOTYPE : struct servent *getservbyname(char *name, char *proto);
SEMANTICS : La funzione getservbyname() accetta come paramaetri il nome di un
            servizio 'name' ed il relativo protocollo 'proto' e restituisce un
            puntatore ad una struttura servent, che fornira' diverse
            informazioni sul servizio, inclusa la porta.
RETURNS   : Un puntatore alla struttura servent in caso di successo, NULL in
            caso di errore.
--------------------------------------------------------------------------------
*/

int main(void)
{
    char* host_name = "www.lwn.com";
    char* service_name = "http";
    char* proto_name = "tcp";
    struct hostent* host_info;
    struct servent* service;
    struct sockaddr_in transport_address;

    int domain = AF_INET;      /* Tipo di indirizzo di trasporto */
    in_addr_t address = 0;
    int port = 0;

    /* Si otengono informazioni sull'host */
    if ((host_info = gethostbyname(host_name)) == NULL) {
        fprintf(stderr, "Err. gethostbyname() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si estrae l'indirizzo dell'host */
    address = *((in_addr_t*)host_info->h_addr_list[0]);

    /* service ora avra' diverse informazioni circa il servizio, tra cui il
    numero di porta */
    if ((service = getservbyname(service_name, proto_name)) == NULL) {
        fprintf(stderr, "Err. getservbyname() \n");
        exit(EXIT_FAILURE);
    }

    /* Il numero di porta */
    port = ntohs(service->s_port);

    transport_address.sin_family = domain;
    transport_address.sin_addr.s_addr = address;
    transport_address.sin_port = htons(port);

    printf("\t%u\t%u\t%u\n", transport_address.sin_family,
           transport_address.sin_addr.s_addr,
           transport_address.sin_port);

    return (EXIT_SUCCESS);
}
