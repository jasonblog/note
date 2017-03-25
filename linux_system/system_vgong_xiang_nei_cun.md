# system V共享内存


system V共享内存和posix共享内存类似，system V共享内存是调用shmget函数和shamat函数。   
shmget函数创建共享内存区，或者访问一个存在的内存区，类似系统调用共享内存的open和posix共享内存shm_open函数。shmget函数原型为：


```c
#include <sys/ipc.h>  
#include <sys/shm.h>  
  
int shmget(key_t key, size_t size, int shmflg); 
```


key: 函数ftok返回值，或者IPC_PRIVATE ,当使用IPC_PRIVATE时，最好两个进程空间是共享的，比如父子进程，否则当前进程产生的共享内存标识（返回值），在另一个进程里面不易得到；


ftok函数原型为：key_t ftok(const char *pathname, int proj_id); 参数pathname为文件绝对路径名，proj_id为一个整型标识符，该函数将一个已存在的的路径名和一个整型标识符转化成一个key_t值（返回值），称为IPC键。

size：创建新的共享内存大小，当创建一片新的共享内存时，该值为不为0的参数。如果是读取一片共享内存，该值可以为0。

shmflg：读写权限值组合。IPC_CREAT（创建新的共享内存）或IPC_CREAT|IPC_EXCL（当将要创建的共享内存已经存在时，再试图创建将返回EEXIST）。其实IPC_CREAT和IPC_EXCL的组合和open函数的O_CREAT和O_EXCL组合类似。

函数返回共享内存区的标识。shmxxx函数操作共享内存将使用该函数返回值。该函数类似posix共享内存shm_open函数功能。

当shmget创建或打开一个共享内存区后，需要使用函数shmat来将该片共享内存连接到当前进程空间中来，当某一进程使用完共享内存后，使用函数shmdt断开和共享内存的链接。


```c
#include <sys/types.h>
#include <sys/shm.h>
void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);  
```

 shmid：是函数shmget函数返回的共享内存标识符。
 
shmaddr： 连接到调用进程地址空间的地址，如果该参数为NULL，系统选择一个合适地址；如果shmaddr非空并且shmflg指定了选项SHM_RND，那么相应的共享内存链接到由shmaddr参数指定的地址向下舍入一个SHMLAB常值。如果shmaddr非空并且shmflg未指定SHM_RND，共享内存地址链接到shmaddr参数指定的地址。

shmflg：可以指定SHM_RND和SHM_RDONLY(只读)，如果指定SHM_RDONLY选项，那么调用进程对该片共享内存只有读权限，否则，进程对该片内存将有读写权限。

```c
#include <sys/ipc.h>
#include <sys/shm.h>
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
```

shmid：共享内存区标识；

cmd：对共享内存的操作命令，命令IPC_RMID销毁（destroy）一片共享内存，销毁之后所有shmat，shmdt，shmctl对该片内存操作都将失效，销毁该共享内存要等到该共享内存引用计数变为0才进行；IPC_SET命令设置shmid_ds结构成员；IPC_STAT返回当前共享内存结构；其余命令查看man手册。

buf：为指向shmid_ds数据结构；


system V 共享内存示例：

server process：

```c
int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int shmid;
    key_t key;


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
    char* shm_file = NULL;
    char* shm_buf = NULL;
    int shmid;


    shmid = sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);

    if (shmid < 0) {
        return -1;
    }

    snprintf(shm_buf, SHM_IPC_MAX_LEN,
             "Hello system V shaare memory IPC! this is write by server.");

    sleep(15);

    printf("System V server delete share memory segment!\n");
    //shmdt(shm_buf);

    shmctl(shmid, IPC_RMID,
           NULL); //server在15秒之后destroy该片共享内存，此时客户进程将获取不到共享内存的内容

    return 0;
}
```

client process：

```c
int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int shmid;
    key_t key;


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
    char* shm_buf = NULL;
    int i;

    if (sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN) < 0) {
        return -1;
    }


    printf("ipc client get: %s\n", shm_buf);


    return 0;
}
```

运行时，首先执行server process，使用命令ipcs可以查看当前系统共享内存：
```sh
# ipcs 
------ Message Queues -------- 
key msqid owner perms used-bytes messages 


------ Shared Memory Segments -------- 
key shmid owner perms bytes nattch status 
0x0010a797 131072 root 0 4096 1 


------ Semaphore Arrays -------- 
key semid owner perms nsems 
```

可以看到存在一个共享内存区，其中key为：0x0010a797 ，共享内存ID为：131072 

```sh
# ./client 
ipc client get: Hello system V shaare memory IPC! this is write by server. 
#
```
当server进程destroy共享内存之后，再重复上面步骤，
```sh
# ipcs 

------ Message Queues -------- 
key msqid owner perms used-bytes messages 


------ Shared Memory Segments -------- 
key shmid owner perms bytes nattch status 


------ Semaphore Arrays -------- 
key semid owner perms nsems 
```

此时共享内存已经不在了，但文件依然存在。

```sh
# ./client
ipc client get: 
```



此时客户端已经不能获取之前共享内存内容了。
另外，ipcrm命令可以在命令行上删除指定共享内存区。
通过读取文件/proc/sys/kernel/shmmax可以获取系统所支持共享内存最大值，

```sh
# cat /proc/sys/kernel/shmmax
33554432
```


可以看到我目前系统支持最大个共享内存值为：32M。


通过上示例可以看到system V共享内存和posix共享内存类似，不过posix共享内存的大小可以随时通过ftruncate改变，而system V 的共享内存大小在shmget时就已经确定下来了。
同样的，system V共享内存大多数时候也需要在多进程之间同步，system V 可以使用自己的信号量来实现，具体细节将在后面同步相关专栏详细讲解。

本节源码下载：

http://download.csdn.net/detail/gentleliu/8140887