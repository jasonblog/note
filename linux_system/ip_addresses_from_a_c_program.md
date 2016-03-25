# IP addresses from a C program


```c
#include <stdio.h>
#include <stropts.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int print_addresses(const int domain)
{
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;

    s = socket(domain, SOCK_STREAM, 0);

    if (s < 0) {
        perror("socket");
        return 0;
    }

    ifconf.ifc_buf = (char*) ifr;
    ifconf.ifc_len = sizeof ifr;

    if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
        perror("ioctl");
        return 0;
    }

    ifs = ifconf.ifc_len / sizeof(ifr[0]);
    printf("interfaces = %d:\n", ifs);

    for (i = 0; i < ifs; i++) {
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in* s_in = (struct sockaddr_in*) &ifr[i].ifr_addr;

        if (!inet_ntop(domain, &s_in->sin_addr, ip, sizeof(ip))) {
            perror("inet_ntop");
            return 0;
        }

        printf("%s - %s\n", ifr[i].ifr_name, ip);
    }

    close(s);

    return 1;
}

int main(int argc, char* argv[])
{
    int domains[] = { AF_INET, AF_INET6 };
    int i;

    for (i = 0; i < sizeof(domains) / sizeof(domains[0]); i++)
        if (!print_addresses(domains[i])) {
            return 1;
        }

    return 0;
}
```