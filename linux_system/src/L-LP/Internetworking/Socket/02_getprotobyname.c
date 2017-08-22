#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

/* La funzione getprotobyname() restituisce un puntatore ad una struttura
protoent contenente campi con informazioni inerenti il protocollo, la lista con
tali informazioni e' collocata in /etc/protocols.

struct protoent {
    char    *p_name;        il nome ufficiale del protocollo
    char    **p_aliases;    lista di alias
    int     p_proto;        numero del protocollo
}

HEADER    : <netdb.h>
PROTOTYPE : struct protoent *getprotobyname(const char *name);
SEMANTICS : La funzione getrpcbyname() ritorna una struttura protoent contenente
            alcune voci sul protocollo definito da 'name'.
RETURNS   : Un puntatore alla struttura protoent in caso di successo, o NULL in
            caso di errore
--------------------------------------------------------------------------------
*/

/* Il programma riceve un parametro da utilizzare con getprotobyname(), se tale
parametro corrisponde ad un protocollo valido saranno restituite alcune
informazioni specifiche relative al protocollo stesso. */

int main(int argc, char* argv[])
{
    int dominio = AF_INET;
    int tipo = SOCK_DGRAM;
    int protocollo = 0;

    /* socket */
    int skt;

    /* Informazioni sul protocollo */
    struct protoent* proto_info;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <protocollo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if ((proto_info = getprotobyname(argv[1])) == NULL) {
        fprintf(stderr, "Err. getprotobyname()\n");
        exit(EXIT_FAILURE);
    }

    printf("Informazioni su \'%s\'\n"
           "- nome: %s\n- alias: %s\n- numero protocollo: %d\n", argv[1],
           proto_info->p_name,
           *proto_info->p_aliases,
           proto_info->p_proto);

    protocollo = proto_info->p_proto;

    /* Creazione della socket */
    if ((skt = socket(dominio, tipo, protocollo)) < 0) {
        fprintf(stderr, "Err. socket() - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
