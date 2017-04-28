#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <string.h>
int main(int argc, char* argv[])
{
    int running = 1;
    int shid;
    int semid;
    int value;
    void* sharem = NULL;

    struct sembuf  sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_flg = SEM_UNDO;

    if ((semid = semget((key_t)123456, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, SETVAL, 0) == -1) {
        printf("sem init error");

        if (semctl(semid, 0, IPC_RMID, 0) != 0) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_FAILURE);
    }

    shid = shmget((key_t)654321, (size_t)2048, 0600 | IPC_CREAT);

    if (shid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }


    sharem = shmat(shid, NULL, 0);

    if (sharem == NULL) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    while (running) {
        if ((value = semctl(semid, 0, GETVAL)) == 0) {
            printf("write data operate\n");
            printf("please input something:");
            scanf("%s", sharem);
            sem_b.sem_op = 1;

            if (semop(semid, &sem_b, 1) == -1) {
                fprintf(stderr, "semaphore_p failed\n");
                exit(EXIT_FAILURE);
            }
        }

        if (strcmp(sharem, "end") == 0) {
            running--;
        }
    }

    shmdt(sharem);
    return 0;
}
