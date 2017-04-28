#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
int sem_id;

void init()
{
    key_t key;
    int ret;
    unsigned short sem_array[2];
    union semun {
        int val;
        struct semid_ds* buf;
        unsigned short* array;
    } arg;

    key = ftok(".", 's');

    sem_id = semget(key, 2, IPC_CREAT | 0644);

    sem_array[0] = 0;           //identify the productor
    sem_array[1] = 100;         //identify the space
    //printf("set the productor init value is 0\nset the space init value is 100\n");
    arg.array = sem_array;
    ret = semctl(sem_id, 0, SETALL, arg);

    if (ret == -1) {
        printf("SETALL failed (%d)\n", errno);
    }

    //printf("\nread the number\n");
    printf("productor init is %d\n", semctl(sem_id, 0, GETVAL));
    printf("space init is %d\n\n", semctl(sem_id, 1, GETVAL));

}
void del()
{
    semctl(sem_id, IPC_RMID, 0);
}

int main(int argc, char* argv[])
{
    struct sembuf sops[2];

    sops[0].sem_num = 0;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;

    sops[1].sem_num = 1;
    sops[1].sem_op = -1;
    sops[1].sem_flg = 0;

    init();


    printf("this is productor\n");

    while (1) {
        printf("\n\nbefore produce:\n");
        printf("productor number is %d\n", semctl(sem_id, 0, GETVAL));
        printf("space number is %d\n", semctl(sem_id, 1, GETVAL));

        semop(sem_id, (struct sembuf*)&sops[1],
              1);     //get the space to instore the productor
        printf("now producing......\n");
        semop(sem_id, (struct sembuf*)&sops[0],
              1);     //now tell the customer can bu cusume
        printf("\nafter produce\n");
        printf("spaces number is %d\n", semctl(sem_id, 1, GETVAL));
        printf("productor number is %d\n", semctl(sem_id, 0, GETVAL));
        sleep(4);
    }

    del();
}
