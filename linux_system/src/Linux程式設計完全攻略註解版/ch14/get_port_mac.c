#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<net/if.h>
// argv[0] portname ,such as argv[0] eth0
int main(int argc, char* argv[])
{
    int i;
    struct ifreq ifreq;
    int sock;
    char mac[32];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error");
        exit(EXIT_FAILURE);
    }

    strcpy(ifreq.ifr_name, argv[1]);

    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
        perror("error:");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 6; i++) {
        sprintf(mac + 3 * i, "%02x:", (unsigned char)ifreq.ifr_hwaddr.sa_data[i]);
    }

    mac[17] = '\0';
    printf("mac addr is: %s\n", mac);
    return 0;
}
