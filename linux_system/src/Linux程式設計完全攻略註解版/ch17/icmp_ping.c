#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>

#define PACKET_SIZE             4096
#define MAX_WAIT_TIME       5
#define MAX_NO_PACKETS      4
#define DATA_LEN            56

char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];

int nsend = 0, nreceived = 0;
struct timeval tvrecv;

struct sockaddr_in from;



void statistics(int signo);
unsigned short cal_chksum(unsigned short* addr, int len);
int pack(int pack_no, int pid);
void send_packet(int sockfd, int pid, struct sockaddr_in dest_addr);
void recv_packet(int sockfd, int pid);
int unpack(char* buf, int len, int pid);
void tv_sub(struct timeval* out, struct timeval* in);


void statistics(int signo)
{
    printf("\n--------------------PING statistics-------------------\n");
    printf("%d packets transmitted, %d received , %%%f lost\n", nsend, nreceived,
           (nsend - nreceived) * 1.0 / nsend * 100);

    exit(1);
}


unsigned short cal_chksum(unsigned short* addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short* w = addr;
    unsigned short answer = 0;

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

    return answer;
}


int pack(int pack_no, int pid)
{
    int i, packsize;
    struct icmp* icmp;
    struct timeval* tval;

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;
    icmp->icmp_id = pid;
    packsize = 8 + DATA_LEN;
    tval = (struct timeval*)icmp->icmp_data;

    gettimeofday(tval, NULL);

    icmp->icmp_cksum = cal_chksum((unsigned short*)icmp, packsize);
    return packsize;
}


void send_packet(int sockfd, int pid, struct sockaddr_in dest_addr)
{
    int packetsize;

    while (nsend < MAX_NO_PACKETS) {
        nsend++;
        packetsize = pack(nsend, pid);

        if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*)&dest_addr,
                   sizeof(dest_addr)) < 0) {
            perror("sendto error");
            continue;
        }

        sleep(1);
    }
}


void recv_packet(int sockfd, int pid)
{
    int n, fromlen;
    extern int errno;
    signal(SIGALRM, statistics);
    fromlen = sizeof(from);

    while (nreceived < nsend) {
        alarm(MAX_WAIT_TIME);

        if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0,
                          (struct sockaddr*)&from, &fromlen)) < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("recvfrom error");
            continue;
        }

        gettimeofday(&tvrecv, NULL);

        if (unpack(recvpacket, n, pid) == -1) {
            continue;
        }

        nreceived++;
    }
}



int unpack(char* buf, int len, int pid)
{
    int i, iphdrlen;
    struct ip* ip;
    struct icmp* icmp;
    struct timeval* tvsend;
    double rtt;

    ip = (struct ip*)buf;
    iphdrlen = ip->ip_hl << 2;
    icmp = (struct icmp*)(buf + iphdrlen);

    len -= iphdrlen;

    if (len < 8) {
        printf("ICMP packets\'s length is less than 8\n");
        return -1;
    }

    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid)) {
        tvsend = (struct timeval*)icmp->icmp_data;
        tv_sub(&tvrecv, tvsend);
        rtt = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000;
        printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n", len,
               inet_ntoa(from.sin_addr), icmp->icmp_seq, ip->ip_ttl, rtt);
    } else {
        return -1;
    }
}


main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in dest_addr;
    pid_t pid;

    struct hostent* host;
    struct protoent* protocol;
    unsigned long inaddr = 0l;
    int waittime = MAX_WAIT_TIME;
    int size = 50 * 1024;

    if (argc < 2) {
        printf("usage:%s hostname/IP address\n", argv[0]);
        exit(1);
    }

    if ((protocol = getprotobyname("icmp")) == NULL) {
        perror("getprotobyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) {
        perror("socket error");
        exit(1);
    }

    setuid(getuid());

    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;

    /*if ping a hostname, get the ip used gethostbyname */
    if ((inaddr = inet_addr(argv[1])) == INADDR_NONE) {
        if ((host = gethostbyname(argv[1])) == NULL) {
            perror("gethostbyname error");
            exit(1);
        }

        memcpy((char*) & (dest_addr.sin_addr), host->h_addr, host->h_length);
    } else {
        memcpy((char*) & (dest_addr.sin_addr), (char*)&inaddr, sizeof(inaddr));
    }

    printf("PING %s(%s): %d bytes data in ICMP packets.\n", argv[1],
           inet_ntoa(dest_addr.sin_addr), DATA_LEN);

    pid = getpid();

    send_packet(sockfd, pid, dest_addr);

    recv_packet(sockfd, pid);

    statistics(SIGALRM);

    close(sockfd);
    return 0;
}


void tv_sub(struct timeval* out, struct timeval* in)
{
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }

    out->tv_sec -= in->tv_sec;
}

