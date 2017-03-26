#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>  //ftok()
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h> //shmat()
#include <unistd.h> //access()
#include <stdlib.h> // exit()
#include <signal.h> //signal()

#include "shmipc.h"

int         sem_clt2ser, sem_ser2clt, sem_mutex_id;

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

    //    semget(shm_key, 1, IPC_CREAT);

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

    if (semtimedop(semid, &sb, 1, &tp) < 0) {
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
    //sb.sem_flg = 0;
    sb.sem_flg = SEM_UNDO;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm lock error! semop: %s\n", strerror(errno));
    }
}

void sln_shm_unlock(int semid)
{
    struct sembuf   sb;

    sb.sem_num = SLN_SEM_NUM;
    sb.sem_op = 1;
    //sb.sem_flg = 0;
    sb.sem_flg = SEM_UNDO;

    if (semop(semid, &sb, 1) < 0) {
        printf("shm unlock error! semop: %s\n", strerror(errno));
    }
}


int sln_seminit(const char* filename, int initval)
{
    int     semid;
    key_t   keyid;

    keyid = ftok(filename, 0);

    if (keyid < 0) {
        fprintf(stderr, "ftok: %s\n", strerror(errno));
        return -1;
    }

    semid = semget(keyid, 1, IPC_CREAT | IPC_EXCL);

    if (semid >= 0) { // create success
        printf("=== new sem create! ===\n");

        if (semctl(semid, 0, SETVAL, initval) < 0) {
            printf("semctl: %s\n", strerror(errno));
            return -1;
        }
    } else if (EEXIST == errno) {  //exist, create failed!
        printf("=== open a exist sem! ===\n");
        semid = semget(keyid, 0, 0);

        if (semid < 0) {
            printf("[%d] - semget: %s\n", __LINE__, strerror(errno));
            return -1;
        }

        if (semctl(semid, 0, SETVAL, initval) < 0) {
            printf("semctl: %s\n", strerror(errno));
            return -1;
        }
    } else { //create failed, other reason
        printf("[%d] - semget: %s,  semid: %d\n", __LINE__, strerror(errno), semid);
    }

    return semid;
}

static void sln_create_not_exist_file(const char* filename)
{
    FILE*        fp = NULL;

    if (access(filename, F_OK) < 0) {
        fp = fopen(filename, "w+");

        if (NULL == fp) {
            fprintf(stderr, "fopen <%s> failed!\n", filename);
            return;
        }

        fclose(fp);
    }
}

static void sigint_func(int sig)
{
    if (semctl(sem_clt2ser, 0, IPC_RMID) < 0) {
        printf("semctl: %s\n", strerror(errno));
    }

    if (semctl(sem_ser2clt, 0, IPC_RMID) < 0) {
        printf("semctl: %s\n", strerror(errno));
    }

    if (semctl(sem_mutex_id, 0, IPC_RMID) < 0) {
        printf("semctl: %s\n", strerror(errno));
    }

    exit(0);
}

int main(int argc, const char* argv[])
{
    char*        str = NULL;

    sln_create_not_exist_file(SLN_SEM_SERWAIT_FILEPATH);
    sln_create_not_exist_file(SLN_SEM_CLTWAIT_FILEPATH);
    sln_create_not_exist_file(SLN_SEM_MUTEX_FILEPATH);

    sem_clt2ser = sln_seminit(SLN_SEM_SERWAIT_FILEPATH, 0);

    if (sem_clt2ser < 0) {
        printf("sync sem init failed!\n");
        return -1;
    }

    sem_ser2clt = sln_seminit(SLN_SEM_CLTWAIT_FILEPATH, 0);

    if (sem_clt2ser < 0) {
        printf("sync sem init failed!\n");
        return -1;
    }

    sem_mutex_id = sln_seminit(SLN_SEM_MUTEX_FILEPATH, 1);

    if (sem_mutex_id < 0) {
        printf("mutex sem init failed!\n");
        return -1;
    }

    //signal(SIGINT, sigint_func);

    for (;;) {
        printf("--------start wait!---semval: %d------\n", semctl(sem_clt2ser, 0,
                GETVAL, 0));
        sln_sem_wait(sem_clt2ser);

        printf("=====stat handle!========\n");
        sln_shm_lock(sem_mutex_id);
        sln_shm_get(SLN_SEM_MUTEX_FILEPATH, (void**)&str, 32);
        sleep(6);
        printf("str: %s\n", str);

        if (shmdt(str) < 0) {
            printf("shmdt: %s\n", strerror(errno));
        }

        sln_shm_unlock(sem_mutex_id);
        printf("=====handle ok!========\n");

        sln_sem_post(sem_ser2clt);
    }

    return 0;
}

