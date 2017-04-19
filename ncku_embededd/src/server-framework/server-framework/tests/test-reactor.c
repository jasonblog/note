/* simple HTTP server using reactor API */

#include "reactor.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* accept connections, for HTTP keep-alive */
void on_data(struct Reactor *reactor, int fd);

struct Reactor r = { .on_data = on_data, .maxfd = 1024 };
static int srvfd;

void on_data(struct Reactor *reactor, int fd)
{
    /* listening socket */
    if (fd == srvfd) {
        int client = 0;
        unsigned int len = 0;
        while ((client = accept(fd, NULL, &len)) > 0)
            reactor_add(&r, client);
        /* reactor is edge triggered, we need to clear the cache. */
    } else {
        char buff[1024];
        ssize_t len;
        static char response[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 12\r\n"
            "Connection: keep-alive\r\n"
            "Keep-Alive: timeout=2\r\n"
            "\r\n"
            "Hello World!\r\n";

        if ((len = recv(fd, buff, sizeof(buff), 0)) > 0)
            len = write(fd, response, strlen(response));
    }
}

/* handle the exit signal (^C) */
void on_sigint(int sig)
{
    reactor_stop(&r);
}

int main(void)
{
    printf("Serving HTTP on 127.0.0.1 port 8080\n"
           "now open up the URL http://localhost:8080 on your browser\n\n");

    /* register the exit signal handler */
    signal(SIGINT, on_sigint);

    /* create a server socket */
    char *port = "8080";
    struct addrinfo hints;
    struct addrinfo *servinfo;        /* point to the results */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      /* don't care IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP stream sockets */
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &servinfo)) {
        perror("address error");
        return -1;
    }

    srvfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                   servinfo->ai_protocol);
    if (srvfd <= 0) {
        perror("address error");
        return -1;
    }

    {
        int optval = 1; /* prevent from address being taken */
        setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof(optval));
    }

    /* bind the address to the socket */
    if (bind(srvfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        perror("bind error");
        return -1;
    }

    /* ensure the socket is non-blocking */
    static int flags;
    if (-1 == (flags = fcntl(srvfd, F_GETFL, 0)))
        flags = 0;
    fcntl(srvfd, F_SETFL, flags | O_NONBLOCK);

    /* listen incoming */
    listen(srvfd, SOMAXCONN);
    if (errno)
        perror("starting. last error was");

    freeaddrinfo(servinfo);
    /* when everything is ready, call the reactor */
    reactor_init(&r);
    reactor_add(&r, srvfd);

    while (reactor_review(&r) >= 0)
        /* wait */ ;

    if (errno)
        perror("\nFinished. last error was");
}
