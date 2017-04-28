#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

//#include "shmemory.h"

int main(int argc, char* argv[])
{
    int running = 1;
    char* shm_p = NULL;
    int shmid;
    int semid;
    int value;

    struct sembuf  sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_flg = SEM_UNDO;

    if ((semid = semget((key_t)123456, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    shmid = shmget((key_t)654321, (size_t)2048, 0600 | IPC_CREAT);

    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shm_p = shmat(shmid, NULL, 0);

    if (shm_p == NULL) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    while (running) {
        if ((value = semctl(semid, 0, GETVAL)) == 1) {
            printf("read data operate\n");
            sem_b.sem_op = -1;

            if (semop(semid, &sem_b, 1) == -1) {
                fprintf(stderr, "semaphore_p failed\n");
                exit(EXIT_FAILURE);
            }

            printf("%s\n", shm_p);
        }

        if (strcmp(shm_p, "end") == 0) {
            running--;
        }
    }

    shmdt(shm_p);

    if (shmctl(shmid, IPC_RMID, 0) != 0) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, IPC_RMID, 0) != 0) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
