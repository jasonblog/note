#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Quando si crea un socket non si specificano indirizzi, la funzione infatti ha
il compito di allocare nel kernel tutto il necessario affinche' si possa aprire
un canale di comunicazione, gli indirizzi invece vegnono allocati mediante
specifiche strutture che l'interfaccia dei socket utilizzera' allorquando la
comunicazione sara' realizzata.

Ciascuna famiglia di protocolli e' caratterizzata dalla propria forma di
indirizzamento e dalla relativa struttura degli indirizzi, tali strutture
sono caratterizzate dal prefisso "sockaddr_".

La struttura generica per gli indirizzi dei socket e' "sockaddr" definita in
<sys/socket.h>:

struct sockaddr {
    sa_family_t     sa_family;
    char            sa_data[14];
};

Tutte le funzioni dei socket che usano gli indirizzi sono definite usando nel
prototipo un puntatore a tale struttura.

Indirizzi IPv4 (32 bit)
-----------------------
La struttura degli indirizzi internet IPv4 e' definita nella struttura
"sockaddr_in" definita in <netinet/in.h>:

struct sockaddr_in {
    sa_family_t     sin_family;     - Internet Protocol AF_INET
    in_port_t       sin_port;       - Address port 16 bit, network byte order
    struct in_addr  sin_addr;       - IPv4 internet address
    char            sin_zero;       - Not used
};

Il campo sid_addr e' una struttura "in_addr":

struct in_addr {
    in_addr_t       s_addr;         - Internet address in network byte order
};

*/

int main(void)
{
    char* host_name = "www.lwn.com";

    int dominio = AF_INET;
    in_addr_t indirizzo = 0;
    int porta = 80;

    struct hostent* host_info = gethostbyname(host_name);

    indirizzo = *((in_addr_t*)host_info->h_addr_list[0]);

    struct sockaddr_in ind_trasporto;

    ind_trasporto.sin_family = dominio;
    ind_trasporto.sin_addr.s_addr = indirizzo;
    ind_trasporto.sin_port = htons(porta);

    printf("\t%u\t%u\t%u\n", ind_trasporto.sin_family,
           ind_trasporto.sin_addr.s_addr,
           ind_trasporto.sin_port);

    return (EXIT_SUCCESS);
}
