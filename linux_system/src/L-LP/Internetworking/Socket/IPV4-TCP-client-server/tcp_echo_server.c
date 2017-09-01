#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Implementazione di un echo server TCP per IPV4

Compilare client e server:
--------------------------
$ gcc tcp_client.c -o client
$ gcc tcp_echo_server.c -o server

Eseguire il server, anche in background:
----------------------------------------
$ ./server 3999 &
[1] 7052

Provare l'echoing e le risposte del server eseguento il client:
---------------------------------------------------------------
$ ./client 10.0.0.13 "Unix network programming" 3999
handling client 10.0.0.13/40059
Ricevuti: Unix network programming
*/

#define BUF_SIZE 512

/* Il numero massimo di richiese in attesa */
static const int MAX_PENDING_REQUEST = 5;

/* Funzione per la gestione del client TCP */
void Handle_TCP_Client(int client_sock);


int main(int argc, char* argv[])
{
    int server_sock;                        /* Socket del server */
    int client_sock;                        /* Socket del client */
    struct sockaddr_in server_address;      /* Indirizzo locale */
    struct sockaddr_in client_address;      /* Indirizzo del client */

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <porta server>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Primo argomento: porta locale del server */
    in_port_t server_port = atoi(argv[1]);

    /* Creazione del socket */
    if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "Err. socket() fallitai - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si deve associare il socket del client all'indirizzo e al numero di porta
    del server, la struttura 'sockaddr_in server_address' contiene tali
    informazioni. Si procede anzitutto all'inizializzazione a 0 della struttura
    stessa.  */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    /* qualsiasi interfaccia */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);       /* Porta locale */

    /* Binding del socket verso indirizzo e porta specificata */
    if (bind(server_sock, (struct sockaddr*) &server_address,
             sizeof(server_address)) < 0) {
        fprintf(stderr, "Err. bind() fallita - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Settaggio del socket per l'ascolto */
    if (listen(server_sock, MAX_PENDING_REQUEST) < 0) {
        fprintf(stderr, "Err. listen() fallita - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (;;) {
        /* Setta la lunghezza della struttura dell'indirizzo del client */
        socklen_t client_address_len = sizeof(client_address);

        /* Attende la connessione del client */
        if ((client_sock = accept(
                               server_sock, (struct sockaddr*) &client_address,
                               &client_address_len)) < 0) {
            fprintf(stderr, "Err. accept() fallita %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Il client e' connesso al server */
        char client_name[INET_ADDRSTRLEN];      /* stringa indirizzo client */

        if (inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_name,
                      sizeof(client_name)) != NULL) {
            printf("handling client %s/%d\n", client_name,
                   ntohs(client_address.sin_port));
        } else {
            puts("Non e' possibile connettersi al client");
        }

        Handle_TCP_Client(client_sock);
    }

    return (EXIT_SUCCESS);
}


void Handle_TCP_Client(int client_sock)
{
    char buf[BUF_SIZE];
    ssize_t num_bytes_received;
    ssize_t num_bytes_sent;

    if ((num_bytes_received = recv(client_sock, buf, BUF_SIZE, 0)) < 0) {
        fprintf(stderr, "Err. recv() fallita %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (num_bytes_received > 0) {
        if ((num_bytes_sent = send(client_sock, buf,
                                   num_bytes_received, 0)) < 0) {
            fprintf(stderr, "Err. send() fallita %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (num_bytes_sent != num_bytes_received) {
            fprintf(stderr, "Err. send() byte invalidi %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if ((num_bytes_received = recv(client_sock, buf, BUF_SIZE, 0)) < 0) {
            fprintf(stderr, "Err. recv() fallita - %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    close(client_sock);
}
