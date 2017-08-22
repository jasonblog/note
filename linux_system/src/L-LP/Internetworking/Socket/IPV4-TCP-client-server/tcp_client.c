#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEF_SERVER_PORT 7
#define BUF_SIZE        512

/* Implementazione di un echo client TCP per IPV4 */

int main(int argc, char* argv[])
{
    int sock;                               /* Il socket */
    int ret_val;                            /* L'indirizzo convertito */
    struct sockaddr_in server_address;      /* l'indirizzo del server */
    ssize_t num_bytes;                      /* Il numero di byte inviati */
    unsigned int total_byte_receveid = 0;   /* Il totale dei byte ricevuti */

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <indirizzo server> <echo> [<porta server>]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Primo argomento: Indirizzo IP del server */
    char* server_IP = argv[1];

    /* Secondo argomento: Stringa echo da inviare al server */
    char* echo_string = argv[2];

    /* Si calcola la lunghezza della stringa di echo, sara' utile in seguito */
    size_t echo_string_len = strlen(echo_string);

    /* Terzo argomento opzionale: La porta del server.
    Se la porta del server viene fornita la si converte in intero mediante la
    funzione atoi(), altrimenti si usera' quella di default, ossia la 7 */
    in_port_t server_port = (argc == 4) ? atoi(argv[3]) : DEF_SERVER_PORT;

    /* Si crea un socket IPv4 basato su stream con protocollo TCP. */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Err. creazione socket - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Per connettersi ad un socket e' necessario specificare l'indirizzo e la
    porta a cui connettersi, la struttura 'sockaddr_in server_address' contiene
    tali informazioni, ciascun campo della struttura stessa peraltro e'
    inizializzato a 0. */
    memset(&server_address, 0, sizeof(server_address));

    /* Si provvede al riempimento della struttura server_address: */
    server_address.sin_family = AF_INET;    /* Famiglia indirizzi IPv4 */

    /* Conversioni degli indirizzi, la funzione inet_pton() converte un
    indirizzo IPv4 o IPv6 da testuale - notazione puntata - a binario 32 bit */
    if ((ret_val = inet_pton(AF_INET, server_IP,
                             &server_address.sin_addr.s_addr)) == 0) {
        fprintf(stderr, "Err. inet_pton() str. invalida %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (ret_val < 0) {
        fprintf(stderr, "Err. inet_pton() fallita %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Conversione da host a network byte order (big-endian) */
    server_address.sin_port = htons(server_port);

    /* Si stabilisce la connessione con il server echo */
    if (connect(sock, (struct sockaddr*)&server_address,
                sizeof(server_address)) < 0) {
        fprintf(stderr, "Err. connect() %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si provvede ad inviare la stringa al server mediante la funzione send(),
    che in caso di esito positivo ritornera' il numero di byte inviati */
    if ((num_bytes = send(sock, echo_string, echo_string_len, 0)) < 0) {
        fprintf(stderr, "Err. send() fallita %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (num_bytes != echo_string_len) {
        fprintf(stderr, "Err. send() %s numero byte errato\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Il client riceve la stessa stringa inviata al server  */
    fputs("Ricevuti: ", stdout);

    while (total_byte_receveid < echo_string_len) {
        char buf[BUF_SIZE];         /* I/O buffer */

        /* Mediante la funzione recv() si ottiene la grandezza del buffer -1 per
        il carattere nullo di terminazione stringa; per cui si sta ottenendo il
        numero dei byte ricevuti */
        if ((num_bytes = recv(sock, buf, BUF_SIZE - 1, 0)) < 0) {
            fprintf(stderr, "Err. recv() fallita %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (num_bytes == 0) {
            fprintf(stderr, "Err. recv() conn. chiusa %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Si calcola il totale dei byte ricevuti */
        total_byte_receveid += num_bytes;

        /* Si aggiunge il carattere nullo di terminazione stringa, per poter
        essere gestita successivamente dalla funzione fputs() */
        buf[num_bytes] = '\0';

        /* Si stampa sullo stdout la stringa ottenuta */
        fputs(buf, stdout);
    }

    /* Si aggiunge l'ultimo carattere linefeed */
    fputc('\n', stdout);

    /* Si chiude il socket */
    close(sock);

    return (EXIT_SUCCESS);
}
