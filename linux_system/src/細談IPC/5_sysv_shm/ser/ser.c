#include <stdio.h>
#include <errno.h>
#include <sys/types.h> //ftok()
#include <sys/ipc.h> //ftok()
#include <sys/shm.h> //shmget()

#include "shm_ipc.h"

int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int     shmid;
    key_t   key;

    if (NULL == fopen(shm_file, "w+")) {
        printf("fopen: %s\n", strerror(errno));
        return -1;
    }

    key = ftok(shm_file, 0);

    if (key < 0) {
        printf("ftok: %s\n", strerror(errno));
        return -1;
    }

    shmid = shmget(key, mem_len, IPC_CREAT);

    if (shmid < 0) {
        printf("shmget: %s\n", strerror(errno));
        return -1;
    }

    *mem = (void*)shmat(shmid, NULL, 0);

    if ((void*) - 1 == *mem) {
        printf("shmat: %s\n", strerror(errno));
        return -1;
    }


    return shmid;
}


int main(int argc, const char* argv[])
{
    char*        shm_file = NULL;
    char*        shm_buf = NULL;
    int         shmid;

    shmid = sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);

    if (shmid < 0) {
        return -1;
    }

    snprintf(shm_buf, SHM_IPC_MAX_LEN,
             "Hello system V shaare memory IPC! this is write by server.");

    sleep(15);

    printf("System V server delete share memory segment!\n");
    //shmdt(shm_buf);

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
