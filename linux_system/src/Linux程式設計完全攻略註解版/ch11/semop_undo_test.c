#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

static int semaphore_p(void);
static int semaphore_v(void);
static int set_semvalue(void);
static int get_semvalue(void);

union semun {
    int val;                            /* value for SETVAL */
    struct semid_ds* buf;               /* buffer for IPC_STAT, IPC_SET */
    unsigned short int* array;          /* array for GETALL, SETALL */
    struct seminfo* __buf;              /* buffer for IPC_INFO */
};

int sem_id;
int main(int argc, char* argv)
{
    pid_t pid;
    int i;
    int value;
    key_t key;
    int status;

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if ((sem_id = semget((key_t)123456, 1, IPC_CREAT | 0770)) == -1) {
            perror("semget");
            exit(EXIT_FAILURE);
        }

        if (!set_semvalue()) {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }

        value = get_semvalue();
        printf("this is child,the current value is %d\n", value);

        if (!semaphore_v()) {
            fprintf(stderr, "Failed to v operator\n");
            exit(EXIT_FAILURE);
        }

        value = get_semvalue();
        printf("the child %d V operator,value=%d\n", i, value);

        printf("child exit success\n");
        exit(EXIT_SUCCESS);
    } else { //parent
        sleep(3);

        if ((sem_id = semget((key_t)123456, 1, IPC_CREAT | 0770)) == -1) {
            perror("semget");
            exit(EXIT_FAILURE);
        }

        value = get_semvalue();
        printf("this is parent ,the current value is %d\n", value);
        printf("the parent will remove the sem\n");

        if (semctl(sem_id, 0, IPC_RMID, (struct msquid_ds*)0) == -1) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }

        return 0;
    }
}

static int set_semvalue(void)
{
    union semun sem_union;
    int value;
    sem_union.val = 5;

    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        return (0);
    }

    printf("set value success,");
    printf("init value is %d\n", get_semvalue());
    return (1);
}

static int get_semvalue(void)
{
    int res;

    if ((res = semctl(sem_id, 0, GETVAL)) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return res;
}

static int semaphore_v(void)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    //sem_b.sem_flg = SEM_UNDO;
    sem_b.sem_flg = 0;

    if (semop(sem_id, &sem_b, 1) == -1) {
        perror("semop");
        return (0);
    }

    return (1);
}
