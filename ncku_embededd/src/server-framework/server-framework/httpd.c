#include <stdio.h>
#include <stdlib.h>

#define THREAD_COUNT 1

#include "protocol-server.h"

static void on_data(server_pt srv, int fd)
{
    static char reply[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 12\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=2\r\n"
        "\r\n"
        "Hello World!";
    char buff[1024];

    if (Server.read(srv, fd, buff, 1024)) {
        Server.write(srv, fd, reply, sizeof(reply));
    }
}

void print_conn(server_pt srv, int fd, void* arg)
{
    printf("- Connection to FD: %d\n", fd);
}

void done_printing(server_pt srv, int fd, void* arg)
{
    fprintf(stderr, "# Total Clients: %lu\n", Server.count(srv, NULL));
}

void timer_task(server_pt srv)
{
    size_t count = Server.each(srv, 0,
                               NULL, print_conn,
                               NULL, done_printing);
    fprintf(stderr, "Clients: %lu\n", count);
}

void on_init(server_pt srv)
{
    Server.run_every(srv, 1000, -1, (void*) timer_task, srv);
}

int main(int argc, char* argv[])
{
    struct Protocol protocol = { .on_data = on_data };
    start_server(.protocol = &protocol,
                 .timeout = 2,
                 .on_init = on_init,
                 .threads = THREAD_COUNT);
    return 0;
}
