#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

/*
Un socket e' un'interfaccia verso l'infrastruttura di comunicazione che il
kernel mette a disposizione dei processi per accedere ai servizi del livello di
trasporto dello stack ISO/OSI; un socket puo' assumere due modalita' di
funzionamento:
1 - connesso    , in relazione diretta con un altro socket, stabilendo un
                  rapporto mittente/destinazione permanente.
2 - non connesso, invia messaggi ad un indirizzo di destinazione in rete.

HEADER    : <sys/socket.h>
PROTOTYPE : int socket(int dominio, int tipo, int protocollo);
SEMANTICS : La funzione socket() definisce il tipo di comunicazione.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
La creazione di un socket è gestita mediante la funzione socket(), che
restituisce un file descriptor mediante il quale e' possibile far riferimento al
socket stesso; i parametri della funzione sono tre:

1 - Dominio   , definisce il dominio del socket, ossia la famiglia di protocolli
                utilizzati, si possono adoperare alcune costanti:
            - AF_UNIX , dominio UNIX, comunicazioni locali;
            - AF_INET , internet, per comunicazioni con protocollo ipv4;
            - AF_INET6, internet versione 6, comunicazioni protocollo ipv6.

            AF sta per Address Family, ossia il formato degli indirizzi
            utilizzati da quel dominio.

2 - Tipo      , fornisce un'astrazione del canale di comunicazione, i cui valori
                possono essere:
            - SOCK_STREAM   , socket byte-stream
            - SOCK_DGRAM    , socket datagram
            - SOCK_RAW      , accesso a funzionalita' di basso livello
            - SOCK_SEQPACKET, canale di comunicazione affidabile,
                          bidirezionale

3 - Protocollo, indica il protocollo da utilizzare, che nei sistemi unix-like
                moderni puo' essere o TCP o UDP; il valore numerico e' associato
        a una costante definita in /etc/prococols, solitamente e'
        impostato a 0, ovvero a internet protocol.
*/
int main(int argc, char* argv[])
{
    int dominio = AF_INET;
    int tipo = SOCK_DGRAM;
    int protocollo = 0;

    /* Il socket */
    int skt;

    /* Informazioni sul protocollo */
    struct protoent* proto_info;

    proto_info = getprotobyname("udp");

    if (proto_info == NULL) {
        perror("getprotobyname");
        return (EXIT_FAILURE);
    }

    /* Si utilizza il campo della struttura protoent p_proto per ricavare il
    parametro relativo al numero del protocollo */
    protocollo = proto_info->p_proto;

    /* Si apre il socket */
    skt = socket(dominio, tipo, protocollo);

    if (skt < 0) {
        perror("socket");
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
