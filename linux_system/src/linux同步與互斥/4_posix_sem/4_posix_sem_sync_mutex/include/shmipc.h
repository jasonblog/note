#ifndef SHMIPC_H_H_
#define SHMIPC_H_H_

#include <semaphore.h>

#define SHM_FILE                    "share_memory_file"

#define SEM_CONSUMER_FILE           "sem_consumer"
#define SEM_PRODUCTOR_FILE          "sem_productor"

#define SEM_MUTEX                   "sln_sem_mutex"

#define SHM_MAX_LEN                 4096

#define SEM_TIMEOUT_SEC             2

#define SLN_MUTEX_SHM_LOCK(shmfile, sem_mutex) do {\
        sem_mutex = sem_open(shmfile, O_RDWR | O_CREAT, 0666, 1);\
        if (SEM_FAILED == sem_mutex) {\
            printf("sem_open(%d): %s\n", __LINE__, strerror(errno));\
        }\
        sem_wait(sem_mutex);\
    }while(0)

#define SLN_MUTEX_SHM_UNLOCK(sem_mutex) do {sem_post(sem_mutex);} while(0)

#endif
