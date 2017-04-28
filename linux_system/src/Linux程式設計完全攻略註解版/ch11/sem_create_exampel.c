#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/types.h>

int main(int argc, char* argv[])
{
    int sem;
    key_t key;

    if ((key = ftok(".", 'B')) == -1) {     //
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((sem = semget(key, 3, IPC_CREAT | 0770)) == -1) { //
        perror("semget");
        exit(EXIT_FAILURE);
    }

    printf("sem1 id is:%d\n", sem);

    semctl(sem, 0, IPC_RMID, (struct msquid_ds*)0);     //
    return 0;
}


