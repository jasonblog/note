#include<arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
int main(int argc, char* argv[])
{
    in_addr_t net;
    struct in_addr net_addr, ret;
    char buf[128];
    inet_pton(AF_INET, "192.168.68.128", &ret);
    inet_ntop(AF_INET, &ret, buf, 128);
    printf("buf=%s\n", buf);
}
