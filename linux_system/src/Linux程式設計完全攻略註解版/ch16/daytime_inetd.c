#include<time.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/syslog.h>

#define MAXLINE     4096    /* max text line length */

int main(int argc, char** argv)
{
    socklen_t       len;
    struct sockaddr* cliaddr;
    char            buff[MAXLINE];
    time_t          ticks;

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    write(0, buff, strlen(buff));
    close(0);   /* close TCP connection */
    exit(0);
}

