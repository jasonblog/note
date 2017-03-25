# （五）：system V信號量的互斥與同步


system V信號量操作類似於posix信號量，但system V信號量的操作要複雜得多，posix信號量使用步驟為sem_init（sem_open）-->sem_wait(sem_post) --> sem_close詳見上一節，system V使用不同的函數。


### 1. 創建和打開信號量函數：semget()。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  
int semget(key_t key, int nsems, int semflg);  
```

key為ftok返回值或IPC_PRIVATE；
nsems為指定信號量集合中信號量的數量，一旦創建就不能更改，需要大於0，如果等於0則方位一個已存在的集合。
semflg為讀寫權限值組合。IPC_CREAT（創建新的信號量集合）或IPC_CREAT|IPC_EXCL（當將要創建的信號量集合已經存在時，再試圖創建將返回EEXIST）。其實IPC_CREAT和IPC_EXCL的組合和open函數的O_CREAT和O_EXCL組合類似。


###  2. 對象信號集合進行操作函數：semctl()。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  
int semctl(int semid, int semnum, int cmd, ...);  
```

semid為信號量標識，函數semget的返回值。
semnum為信號在信號集合中的序號。
cmd為操作的命令：
```sh
GETVAL - 返回指定信號量當前值；
SETVAL - 設置指定信號量的值；
IPC_RMIN - 刪除信號量集合。
```
其餘命令詳見man手冊。

初始化一個system V信號量如下：

```c
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

    if (semid >=
        0) { // 創建成功，然後修改信號量的初始值為initval
        printf("=== new sem create! ===\n");

        if (semctl(semid, 0, SETVAL, initval) < 0) {
            printf("semctl: %s\n", strerror(errno));
            return -1;
        }
    } else if (EEXIST ==
               errno) {  //信號量存在，則打開，再次修改信號量的初始值。
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
```

### 3.信號量PV操作函數：semop()，semtimedop()函數。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  

int semop(int semid, struct sembuf *sops, unsigned nsops);  
int semtimedop(int semid, struct sembuf *sops, unsigned nsops, struct timespec *timeout);  ```

semid為信號量標識，函數semget的返回值；<br>
sops指向信號量操作結構數組指針；<br>
nsops為指向數組中sembuf結構體個數。<br>


```c
struct sembuf {  
    short sem_num;    // 要操作的信號量在信號量集裡的編號，  
    short sem_op;     // 信號量操作  
    short sem_flg;     // 操作表示符  
};  
```

若sem_op 是正數，其值就加到semval上，即釋放信號量控制的資源

若sem_op 是0，那麼調用者希望等到semval變為0，如果semval是0就返回;

若sem_op 是負數，那麼調用者希望等待semval變為大於或等於sem_op的絕對值

P、V操作的實現為：

```c
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
```


示例代碼：
服務進程：

```c
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
```

客戶進程代碼：

```c
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
```
http://download.csdn.net/detail/gentleliu/8329425
