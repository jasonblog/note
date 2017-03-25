#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

#include "shmipc.h"

void clear_exist_sem(sem_t* sem)
{
    int                 semval;

    sem_getvalue(sem, &semval);
    printf("========%d=======\n", semval);

    while (semval-- > 0) {
        sem_wait(sem);
    }
}

int nms_shm_get(char* shm_file, void** shm, int mem_len)
{
    int         fd;

    fd = shm_open(shm_file, O_RDWR, 0);

    if (fd < 0) {
        printf("shm_open <%s> failed: %s\n", shm_file, strerror(errno));
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
    sem_t*               sem_consumer = NULL, *sem_productor = NULL;
    struct timespec     timeout;
    int                 ret;
    char*                sharememory = NULL;
    sem_t*               sem_mutex;

    sem_consumer = sem_open(SEM_CONSUMER_FILE, O_RDWR);

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE, O_RDWR);

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    //clear_exist_sem(sem_productor);

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX, sem_mutex);

    nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
    printf("sharememory: %s\n", sharememory);

    SLN_MUTEX_SHM_UNLOCK(sem_mutex);

    sem_post(sem_consumer);

    printf("Post...\n");
    sem_wait(sem_productor);

    /*
    timeout.tv_sec = time(NULL) + SEM_TIMEOUT_SEC;
    timeout.tv_nsec = 0;

    ret = sem_timedwait(sem_productor, &timeout);
    if (ret < 0) {
        printf("sem_timedwait: %s\n", strerror(errno));
    }
    */
    printf("Get response...\n");

    sem_close(sem_consumer);
    sem_close(sem_productor);
    return 0;
}
