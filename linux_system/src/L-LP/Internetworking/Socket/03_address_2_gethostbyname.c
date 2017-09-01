#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>

/* La funzione gethostbyname() resituisce un puntatore alla struttura hostent,
gia' allocata in memoria, con diverse voci inerenti l'hostname:

struct hostent {
    char    *name;
    char    **aliases;
    int     h_addrtype;
    int     h_length;
    char    **h_addr_list;
};

HEADER    : <netdb.h>
PROTOTYPE : struct hostent *gethostbyname(const char *name);
SEMANTICS : La funzione gethostbyname() restituisce un puntatore alla struttura
            hostent, contenente informazioni utili sull'host 'name'.
RETURNS   : Un puntatore alla struttura hostent in caso di successo, o NULL in
            caso di errore
--------------------------------------------------------------------------------
Tale funzione e' considerata obsoleta, si consiglia l'utilizzo di getaddrinfo()
e getnameinfo().
*/

int main(int argc, char* argv[])
{
    int i;

    /* Forma alfanumerica di un indirizzo */
    char* ind_alphanum = "www.lwn.com";

    /* I sistemi operativi che utilizzano lo standard POSIX conservano gli
    indirizzi in una variabile di tipo in_addr_t */
    in_addr_t ind;

    /* La funzione gethostbyname() restituisce un puntatore alla struttura
    hostent, contenente svariate informazioni relative all'host, nel caso
    specifico le informazioni riguardano l'host ind_alphanum  */
    struct hostent* host_info;

    host_info = gethostbyname(ind_alphanum);

    if (host_info == NULL) {
        perror("gethostbyname");
        return -1;
    }

    /* 1° metodo:
    Si estrae il primo indirizzo numerico mediante un cast */
    ind = *((in_addr_t*)(host_info->h_addr_list[0]));

    /* Essendo in_addr_t un unsigned int, si fornisce alla printf lo
    specificatore di conversione %u */
    printf("1° metodo - Indirizzo IP 32bit: %u\n", ind);

    /* 2° Metodo:
    Qualora si volessero estrarre tutti gli indirizzi possibili, sarebbe
    necessario far ciclare l'array puntato da h_addr_list; si noti inoltre una
    ulteriore modalita' per eseguire il cast */
    for (i = 0; host_info->h_addr_list[i] != NULL; i++) {
        int indirizzo = *((int*)(host_info->h_addr_list[i]));
        printf("2° metodo - %d Indirizzo IP 32bit: %u\n", i, indirizzo);
    }

    return (EXIT_SUCCESS);
}
