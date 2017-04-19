#include "protocol-server.h"

#include <stdio.h>
#include <string.h>

void on_open(server_pt server, int sockfd)
{
    printf("A connection was accepted on socket #%d.\n", sockfd);
    const char greeting[] = "\nSimple echo server. Type 'bye' to exit.\n";
    Server.write(server, sockfd, (char *) greeting, sizeof(greeting));
}

void on_close(server_pt server, int sockfd)
{
    printf("Socket #%d is now disconnected.\n", sockfd);
}

/* simple echo, the main callback */
void on_data(server_pt server, int sockfd)
{
    char buff[1024];

    ssize_t incoming = 0;
    /* Read everything, this is edge triggered, `on_data` won't be called
     * again until all the data was read.
     */
    while ((incoming = Server.read(server, sockfd, buff, 1024)) > 0) {
        if (!memcmp(buff, "bye", 3)) {
            /* close the connection automatically AFTER buffer was sent */
            Server.close(server, sockfd);
        } else {
            /* since the data is stack allocated, we'll write a copy
             * optionally, we could avoid a copy using Server.write_move
	     */
            Server.write(server, sockfd, buff, incoming);
        }
    }
}

int main(void)
{
    /* create the echo protocol object as the server's default */
    struct Protocol protocol = {
        .on_open = on_open,
        .on_close = on_close,
        .on_data = on_data,
        .service = "echo"
    };
    start_server(.protocol = &protocol, .timeout = 10, .threads = 8);
}
