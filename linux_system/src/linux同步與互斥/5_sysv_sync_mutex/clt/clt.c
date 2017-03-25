#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>  //ftok()
#include <sys/sem.h>

#include "shmipc.h"


int sln_shm_get(const char* filename, void** mem, int mem_len)
{
    int     shmid;
    key_t   shm_key;

    shm_key = ftok(filename, 0);

    if (shm_key < 0) {
        fprintf(stderr, "ftok: %s\n", strerror(errno));
        return -1;
    }

    shmid = shmget(shm_key, mem_len, IPC_CREAT);

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

void sln_sem_wait_timeout(int semid)
{
    struct sembuf   sb;
    struct timespec tp;

    memset(&sb, 0, sizeof(struct sembuf));

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    tp.tv_sec = time(NULL) + SEM_TIMEOUT_SEC;
    tp.tv_nsec = 0;

    /*
    if (semtimedop(semid, &sb, 1, &tp) < 0) {
        printf("shm lock error! semop: %s\n", strerror(errno));
    }
    */
    if (semop(semid, &sb, 1) < 0) {
        printf("shm lock error! semop: %s\n", strerror(errno));
    }
}

void sln_sem_wait(int semid)
{
    struct sembuf   sb;

    memset(&sb, 0, sizeof(struct sembuf));

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm lock error! semop: %s\n", strerror(errno));
    }
}

void sln_sem_post(int semid)
{
    struct sembuf   sb;

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    //sb.sem_flg = SEM_UNDO;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm unlock error! semop: %s\n", strerror(errno));
    }
}

void sln_shm_lock(int semid)
{
    struct sembuf   sb;

    memset(&sb, 0, sizeof(struct sembuf));

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm lock error! semop: %s\n", strerror(errno));
    }
}

void sln_shm_unlock(int semid)
{
    struct sembuf   sb;

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm unlock error! semop: %s\n", strerror(errno));
    }
}

int sln_semget(const char* filename)
{
    int     semid;
    key_t   keyid;

    keyid = ftok(filename, 0);

    if (keyid < 0) {
        fprintf(stderr, "ftok: %s\n", strerror(errno));
        return -1;
    }

    return semget(keyid, 0, 0);
}

int main(int argc, const char* argv[])
{
    char*        str = NULL;
    int         sem_clt2ser, sem_ser2clt, sem_mutex_id;

    sem_clt2ser = sln_semget(SLN_SEM_SERWAIT_FILEPATH);

    if (sem_clt2ser < 0) {
        printf("semget: %s\n", strerror(errno));
        return -1;
    }

    sem_ser2clt = sln_semget(SLN_SEM_CLTWAIT_FILEPATH);

    if (sem_ser2clt < 0) {
        printf("semget: %s\n", strerror(errno));
        return -1;
    }

    sem_mutex_id = sln_semget(SLN_SEM_MUTEX_FILEPATH);

    if (sem_mutex_id < 0) {
        printf("mutex sem failed!\n");
        return -1;
    }

    printf("--------cli wait!-sem_mutex_id: %d--------\n", sem_mutex_id);
    sln_shm_lock(sem_mutex_id);
    printf("--------cli wait ok !---------\n");

    sln_shm_get(SLN_SEM_MUTEX_FILEPATH, (void**)&str, 32);
    strcpy(str, argv[1]);

    sln_shm_unlock(sem_mutex_id);

    sln_sem_post(sem_clt2ser);
    sln_sem_wait(sem_ser2clt);
    printf("=========set: %s=========\n", str);

    return 0;
}
