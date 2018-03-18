#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
int main(int argc, char* argv[])
{
    int rc;
    char ipbuf[16];

    struct addrinfo hints, *addr;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG;

    if ((rc = getaddrinfo(argv[1], argv[2], &hints, &addr)) == 0) {
        do {
            printf("ip: %s\t",
                   inet_ntop(AF_INET,
                             &(((struct sockaddr_in*)addr->ai_addr)->sin_addr),
                             ipbuf,
                             sizeof(ipbuf)));
            printf("host:%s\t", addr->ai_canonname);
            printf("length:%d\t", addr->ai_addrlen);
            printf("port:%d\n", ntohs(((struct sockaddr_in*)addr->ai_addr)->sin_port));
        } while ((addr = addr->ai_next) != NULL);

        return 0;
    }

    printf("%d\n", rc);
    return 0;
}

