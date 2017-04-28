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
    memset(buf, '\0', 128);
    net = inet_addr("192.168.68.128");
    net_addr.s_addr = net;

    printf("inet_addr(192.168.68.128)=0x%x\n", inet_addr("192.168.68.128"));
    printf("inet_network(192.168.68.128)=0x%x\n", inet_network("192.168.68.128"));

    printf("inet_ntoa(net)%s\n", inet_ntoa(net_addr));
    inet_aton("192.168.68.128", &ret);
    printf("test inet_aton,then inet_ntoa(ret)%s\n", inet_ntoa(ret));
}
