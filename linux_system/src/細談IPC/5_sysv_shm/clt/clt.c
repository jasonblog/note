#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>  //ftok()
#include <sys/shm.h>

#include "shm_ipc.h"


int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int     shmid;
    key_t   key;

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
    char*        shm_buf = NULL;
    int         i;

    if (sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN) < 0) {
        return -1;
    }

    printf("ipc client get: %s\n", shm_buf);



    return 0;
}
