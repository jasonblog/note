#ifndef SHMIPC_H_H_
#define SHMIPC_H_H_

#include <semaphore.h>

#define SLN_SEM_SERWAIT_FILEPATH     "/dev/shm/sync_serwait_file"
#define SLN_SEM_CLTWAIT_FILEPATH     "/dev/shm/sync_cltwait_file"
#define SLN_SEM_MUTEX_FILEPATH    "/dev/shm/mutex_file"
#define SLN_SEM_NUM     0

#define SEM_TIMEOUT_SEC    2

typedef struct _shmipc_t {
    int     type;
    int     val;
    char    data[16];
} shmipc_t;

#endif
