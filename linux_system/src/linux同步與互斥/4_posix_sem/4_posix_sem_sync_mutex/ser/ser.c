#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "shmipc.h"


int nms_shm_get(char* shm_file, void** shm, int mem_len)
{
    int         fd;

    fd = shm_open(shm_file, O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
        printf("shm_pen <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);

    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == *shm) {
        printf("mmap: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    sem_t*       sem_mutex = NULL;
    sem_t*       sem_consumer = NULL, *sem_productor = NULL;
    int         semval;
    char*        sharememory = NULL;

    sem_consumer = sem_open(SEM_CONSUMER_FILE, O_CREAT, 0666, 0);

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE, O_CREAT, 0666, 0);

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    for (;;) {
        sem_getvalue(sem_consumer, &semval);
        printf("%d waiting...\n", semval);

        if (sem_wait(sem_consumer) < 0) {
            printf("sem_wait: %s\n", strerror(errno));
            return -1;
        }

        printf("Get request...\n");

        SLN_MUTEX_SHM_LOCK(SEM_MUTEX, sem_mutex);
        nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
        sleep(6);
        snprintf(sharememory, SHM_MAX_LEN, "Hello, this is server's message!");
        SLN_MUTEX_SHM_UNLOCK(sem_mutex);

        sem_post(sem_productor);
        printf("Response request...\n");
    }

    sem_close(sem_consumer);
    sem_close(sem_productor);
    return 0;
}
