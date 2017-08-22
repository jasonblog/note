#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Il programma crea un processo figlio, il quale stampa a video i processi
relativi all'utente 'user_name', ma solo alcune informazioni, dopodiche' il
figlio esce normalmente e il padre, dopo aver atteso la conclusione del figlio,
termina, eventualmente un processo */

int main(int argc, char* argv[])
{
    pid_t pid, pid_kill;
    int child_status;
    char* user_name;

    if ((user_name = getenv("USER")) == NULL) {
        fprintf(stderr, "Err.(%s) getenv() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char* arg_var[] = {
        "ps",
        "-U",
        user_name,
        "-o",
        "pid,ppid,ruid,euid,args",
        (char*)0
    };

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (execv("/bin/ps", arg_var) < 0) {
            fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        if (waitpid(-1, &child_status, 0) < 0) {
            fprintf(stderr, "Err.(%s) waitpid() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("get PID to kill (0=quit): ");
        scanf("%d", &pid_kill);

        if (pid_kill == 0) {
            printf("bye\n");
            exit(0);
        }

        if (kill(pid_kill, SIGKILL) == -1) {
            fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
