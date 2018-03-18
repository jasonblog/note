#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#define MAX_SEMAPHORES  5
int main(int argc, char* argv[])
{
    int i, ret, semid;
    unsigned short sem_array[MAX_SEMAPHORES];
    unsigned short sem_read_array[MAX_SEMAPHORES];

    union semun {
        int val;
        struct semid_ds* buf;
        unsigned short* array;
    } arg;

    semid = semget(IPC_PRIVATE, MAX_SEMAPHORES, IPC_CREAT | 0666);

    if (semid != -1) {
        /* Initialize the sem_array */
        for (i = 0 ; i < MAX_SEMAPHORES ; i++) {
            sem_array[i] = (unsigned short)(i + 1);
        }

        /* Update the arg union with the sem_array address */
        arg.array = sem_array;
        /* Set the values of the semaphore-array */
        ret = semctl(semid, 0, SETALL, arg);

        if (ret == -1) {
            printf("SETALL failed (%d)\n", errno);
        }

        /* Update the arg union with another array for read */
        arg.array = sem_read_array;
        /* Read the values of the semaphore array */
        ret = semctl(semid, 0, GETALL, arg);

        if (ret == -1) {
            printf("GETALL failed (%d)\n", errno);
        }

        /* print the sem_read_array */
        for (i = 0 ; i < MAX_SEMAPHORES ; i++) {
            printf("Semaphore %d, value %d\n", i, sem_read_array[i]);

        }

        /* Use GETVAL in a similar manner */
        for (i = 0 ; i < MAX_SEMAPHORES ; i++) {
            ret = semctl(semid, i, GETVAL);
            printf("Semaphore %d, value %d\n", i, ret);
        }

        /* Delete the semaphore */
        ret = semctl(semid, 0, IPC_RMID);
    } else {
        printf("Could not allocate semaphore (%d)\n", errno);
    }

    return 0;
}
