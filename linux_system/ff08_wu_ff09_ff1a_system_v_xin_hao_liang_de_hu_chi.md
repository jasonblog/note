# （五）：system V信号量的互斥与同步


system V信号量操作类似于posix信号量，但system V信号量的操作要复杂得多，posix信号量使用步骤为sem_init（sem_open）-->sem_wait(sem_post) --> sem_close详见上一节，system V使用不同的函数。


### 1. 创建和打开信号量函数：semget()。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  
int semget(key_t key, int nsems, int semflg);  
```

key为ftok返回值或IPC_PRIVATE；
nsems为指定信号量集合中信号量的数量，一旦创建就不能更改，需要大于0，如果等于0则方位一个已存在的集合。
semflg为读写权限值组合。IPC_CREAT（创建新的信号量集合）或IPC_CREAT|IPC_EXCL（当将要创建的信号量集合已经存在时，再试图创建将返回EEXIST）。其实IPC_CREAT和IPC_EXCL的组合和open函数的O_CREAT和O_EXCL组合类似。


###  2. 对象信号集合进行操作函数：semctl()。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  
int semctl(int semid, int semnum, int cmd, ...);  
```

semid为信号量标识，函数semget的返回值。
semnum为信号在信号集合中的序号。
cmd为操作的命令：
```sh
GETVAL - 返回指定信号量当前值；
SETVAL - 设置指定信号量的值；
IPC_RMIN - 删除信号量集合。
```
其余命令详见man手册。

初始化一个system V信号量如下：

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
        0) { // 创建成功，然后修改信号量的初始值为initval
        printf("=== new sem create! ===\n");

        if (semctl(semid, 0, SETVAL, initval) < 0) {
            printf("semctl: %s\n", strerror(errno));
            return -1;
        }
    } else if (EEXIST ==
               errno) {  //信号量存在，则打开，再次修改信号量的初始值。
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

### 3.信号量PV操作函数：semop()，semtimedop()函数。

```c
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/sem.h>  

int semop(int semid, struct sembuf *sops, unsigned nsops);  
int semtimedop(int semid, struct sembuf *sops, unsigned nsops, struct timespec *timeout);  ```

semid为信号量标识，函数semget的返回值；<br>
sops指向信号量操作结构数组指针；<br>
nsops为指向数组中sembuf结构体个数。<br>


```c
struct sembuf {  
    short sem_num;    // 要操作的信号量在信号量集里的编号，  
    short sem_op;     // 信号量操作  
    short sem_flg;     // 操作表示符  
};  
```

若sem_op 是正数，其值就加到semval上，即释放信号量控制的资源

若sem_op 是0，那么调用者希望等到semval变为0，如果semval是0就返回;

若sem_op 是负数，那么调用者希望等待semval变为大于或等于sem_op的绝对值

P、V操作的实现为：

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


示例代码：
服务进程：

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

客户进程代码：

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
