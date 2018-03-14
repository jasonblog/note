
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PROCESS_NUM 10
#define MAXEVENTS 64

int main (int argc, char *argv[])
{
    int sfd, efd;
    int flags;
    int n, i, k;
    struct epoll_event event;
    struct epoll_event *events;
    struct sockaddr_in serveraddr;

    for(k = 0; k < PROCESS_NUM; k++) {
        if (fork() == 0) { /* children process */
			sfd = socket(PF_INET, SOCK_STREAM, 0);
			memset(&serveraddr, 0, sizeof(serveraddr));
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
			serveraddr.sin_port = htons(atoi("1234"));

			flags = fcntl (sfd, F_GETFL, 0);
			fcntl (sfd, F_SETFL, flags | O_NONBLOCK |SO_REUSEPORT);
			bind(sfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

			if (listen(sfd, SOMAXCONN) < 0) {
				perror ("listen");
				exit(EXIT_SUCCESS);
			}

			if ((efd = epoll_create(MAXEVENTS)) < 0) {
				perror("epoll_create");
				exit(EXIT_SUCCESS);
			}

			event.data.fd = sfd;
			event.events = EPOLLIN; // | EPOLLET;
			if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) < 0) {
				perror("epoll_ctl");
				exit(EXIT_SUCCESS);
			}

			/* Buffer where events are returned */
			events = (struct epoll_event*)calloc(MAXEVENTS, sizeof event);

            while (1) {    /* The event loop */
                n = epoll_wait(efd, events, MAXEVENTS, -1);
                printf("process #%d return from epoll_wait!\n", getpid());
                for (i = 0; i < n; i++) {
                    if ((events[i].events & EPOLLERR) ||
                        (events[i].events & EPOLLHUP) ||
                        (!(events[i].events & EPOLLIN))) {
                        /* An error has occured on this fd, or the socket is not
                         * ready for reading (why were we notified then?)
                         */
                        fprintf (stderr, "epoll error\n");
                        close (events[i].data.fd);
                        continue;
                    } else if (sfd == events[i].data.fd) {
                        /* We have a notification on the listening socket, which
                         * means one or more incoming connections.
                         */
                        struct sockaddr in_addr;
                        socklen_t in_len;
                        int infd;
                        //char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                        in_len = sizeof in_addr;

                        infd = accept(sfd, &in_addr, &in_len);
                        if (infd == -1) {
                            printf("process %d accept failed!\n", getpid());
                            break;
                        }
                        printf("process %d accept successed!\n", getpid());
						sleep(20);  /* sleep here is very important!*/

                        /* Make the incoming socket non-blocking and add it to the
                        list of fds to monitor. */
                        close(infd);
                    }
                }
            }
			free (events);
			close (sfd);
        }
    }
    int status;
    wait(&status);
    return EXIT_SUCCESS;
}
