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

    key = ftok(".", 's');
    sem_id = semget(key, 2, IPC_CREAT | 0644);
    //printf("sem id is %d\n",sem_id);
}
int main(int argc, char* argv[])
{

    init();

    struct sembuf sops[2];

    sops[0].sem_num = 0;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    sops[1].sem_num = 1;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    init();


    printf("this is customer\n");

    while (1) {
        printf("\n\nbefore consume:\n");
        printf("productor is %d\n", semctl(sem_id, 0, GETVAL));
        printf("space  is %d\n", semctl(sem_id, 1, GETVAL));

        semop(sem_id, (struct sembuf*)&sops[0], 1);     //get the productor to cusume
        printf("now consuming......\n");
        semop(sem_id, (struct sembuf*)&sops[1],
              1);     //now tell the productor can bu produce

        printf("\nafter consume\n");
        printf("products number is %d\n", semctl(sem_id, 0, GETVAL));
        printf("space number is %d\n", semctl(sem_id, 1, GETVAL));
        sleep(3);
    }
}
