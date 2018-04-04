#include"test_httpd.h"

int main(int argc, char** argv)
{
    struct sockaddr_in addr;
    int sock_fd, addrlen;

    init_daemon(argv[0], LOG_INFO);

    if (get_arg("home_dir") == 0) {
        sprintf(home_dir, "%s", "/tmp");
    }

    if (get_arg("ip") == 0) {
        get_addr("eth0");
    }

    if (get_arg("port") == 0) {
        sprintf(port, "%s", "80");
    }

    if (get_arg("back") == 0) {
        sprintf(back, "%s", "5");
    }

    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        wrtinfomsg("socket()");
        exit(EXIT_FAILURE);
    }

    addrlen = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen, sizeof(addrlen));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);
    addrlen = sizeof(struct sockaddr_in);

    if (bind(sock_fd, (struct sockaddr*) &addr, addrlen) < 0) {
        wrtinfomsg("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, atoi(back)) < 0) {
        wrtinfomsg("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int len;
        int new_fd;
        addrlen = sizeof(struct sockaddr_in);

        new_fd = accept(sock_fd, (struct sockaddr*) &addr, &addrlen);

        if (new_fd < 0) {
            wrtinfomsg("accept");
            exit(EXIT_FAILURE);
        }

        bzero(buffer, MAXBUF + 1);
        sprintf(buffer, "connect come from: %s:%d\n",
                inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        wrtinfomsg(buffer);

        //fork a new process to deal with the connect ,the parent continue wait for new connect
        pid_t pid;

        if ((pid = fork()) == -1) {
            wrtinfomsg("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            close(sock_fd);
            bzero(buffer, MAXBUF + 1);

            if ((len = recv(new_fd, buffer, MAXBUF, 0)) > 0) {
                FILE* ClientFP = fdopen(new_fd, "w");

                if (ClientFP == NULL) {
                    wrtinfomsg("fdopen");
                    exit(EXIT_FAILURE);
                } else {
                    char Req[MAXPATH + 1] = "";
                    sscanf(buffer, "GET %s HTTP", Req);
                    bzero(buffer, MAXBUF + 1);
                    sprintf(buffer, "Reuquest get the file: \"%s\"\n", Req);
                    wrtinfomsg(buffer);
                    GiveResponse(ClientFP, Req);
                    fclose(ClientFP);
                }
            }

            exit(EXIT_SUCCESS);
        } else {
            close(new_fd);
            continue;
        }
    }

    close(sock_fd);
    return 0;
}

